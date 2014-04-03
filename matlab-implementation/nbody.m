function [potential,tree] = nbody(points,densities,maxPntsPerNode,maxLevel,plottree)
% function [u] = run(this,points,densities,maxPntsPerNode,maxLevel)
%
% points         : dim X number_of_points array that holds the coordinates of points
% densities      : input densites (array of size num_of_points); all densities must be positive
% maxPntsPerNode : used to decide the subdivision of a node
% maxLevel       : maximum tree level  
% plottree       : (logical) if true, the tree traversal is visualized
%
% potential      : the computed potential at each point
% tree           : a pointer to the root of the tree  
%  
%  here we assume that we only compute self interactions, that is the source and target 
%  points coincide.

if nargin<1, selftest; return; end
if nargin<3, maxPntsPerNode=1; end;
if nargin<4, maxLevel=20;      end;
if nargin<5, plottree=false;   end;
assert(all(densities>=0),'All densities must be positive');
% remark: the algorithm applies to arbitrary densities, but the simple
%         averaging implemented here works only for positive densities.

% BUILD TREE ---------------------------------------
gids = 1:size(points,2);
root = qtree;
root.insert_points(gids,points,maxPntsPerNode,maxLevel);

user_data.points    = points;
user_data.densities = densities;
user_data.plottree  = plottree; % to plot tree as you evaluate()

% collect all the leaves on a single lits
% used in averaging and evaluation to expose parallelism  
leaves = root.leaves(); 

% AVERAGE  ---------------------------------------
% we could use the commented line below, but for many points per box
% the loop version can be parallelized for the leaf nodes
%root.postorder(@average_leaves,[], user_data);
for l=1:length(leaves)
  leaf = leaves{l};
  average_leaves(leaf,user_data);
end
% average internal nodes
root.postorder(@average_internal,[],[]);

% EVALUATE  ---------------------------------------
u = zeros(size(points,2),1);
for l = 1:length(leaves)
  if(plottree) pause(0.05); clf; hold on; end
  leaf = leaves{l};
  if ~isempty(leaf.gids)
    u(leaf.gids) = evaluate( leaf, root, user_data);
  end
end

potential = u;
tree      = root;
end


%/* ************************************************** */
function u = evaluate(leaf,root,ud)
% the main evaluation routine for the NBODY   

  if ud.plottree, leaf.plotnode(4); end  % optional plot
  if isempty(leaf.gids) return; end;     % if leaf has no points nothing to do
  trg = ud.points(:,leaf.gids);          % get target point coordinates
  u   = zeros(length(leaf.gids),1);      % initialize potential to zero
  
% prunning function,  a handle function to use for traversals
  prune=@(node,dummy) well_separated(node,leaf); 

% evaluation function, again to be used in the traversals
  function visit(node,ud)                           
    if ud.plottree, node.plotnode(); pause(0.01); end  % optional plot
    if node.isleaf & ~isempty(node.gids)  % if source box has not sources, nothing to do
      src = ud.points(:,node.gids);       % get source positions
      den = ud.densities(:,node.gids);    % get source densities values
      u = u + direct(trg,src,den);        % direct evaluation between boxes
      return;
    end
  % if we prune, we have to add the nodes contribution to the leaf
    if prune(node,[]) 
      u = u + direct(trg, node.data.x_a, node.data.d_a);  % direct evaluat
    end
  end

% preorder traversal for evaluation
  root.preorder(@visit,prune,ud);   % if couldn't prune, continue with traversal
end


%/* ************************************************** */
function average_leaves(node,user_data)
  if ~node.isleaf, return; end;
  points    = user_data.points;
  densities = user_data.densities;
  gids = node.gids;

  if isempty(gids) % empty leaf
    x_a=zeros(2,1); d_a=0; 
  else
    d_a = sum( densities(gids) );  % average 
    assert(d_a>0,'Only positive densities allowed');
    x_a(1) = sum( points   (1,gids).*densities(gids) )/d_a;  % average position
    x_a(2) = sum( points   (2,gids).*densities(gids) )/d_a;  % average position
  end
  node.data.x_a = x_a(:);
  node.data.d_a = d_a;
end

%/* ************************************************** */
function average_internal(node,user_data)
if node.isleaf, return; end;
  x_a = zeros(2,1); 
  d_a = 0;
  for k=1:4
    d_a = d_a + node.kids{k}.data.d_a;
    x_a = x_a + node.kids{k}.data.x_a * node.kids{k}.data.d_a;
  end
  node.data.x_a = x_a/d_a;
  node.data.d_a = d_a;
  if ~(d_a > 0) x_a = zeros(2,1); end  % if we have empty leaves d_a=0
end


%/* ************************************************** */
function itis=well_separated(source,target)
% check whether two boxes are well separated  
  [t_xmin,t_xmax,t_ymin,t_ymax] = target.corners();
  [s_xmin,s_xmax,s_ymin,s_ymax] = source.corners();
  h = source.width();
  
% neighbor region of target
  s_xmin = s_xmin -h;   s_ymin = s_ymin -h;
  s_xmax = s_xmax +h;   s_ymax = s_ymax +h;

% check overlap of source box neigbhorhood region with target
  flagx = t_xmax > s_xmin & t_xmin < s_xmax;
  flagy = t_ymax > s_ymin & t_ymin < s_ymax;

% if both flags  are true the boxes are not well separated
  itis = ~ (flagx & flagy);
end


%/* ************************************************** */
function u = direct(trg,src,densities)
% this is a direct N^2 evaluation between particles  
  n = size(trg,2);
  u = zeros(n,1);

% MAIN LOOP OVER POINTS
  for k=1:n
  % compute distance  
    rx = trg(1,k) - src(1,:);  
    ry = trg(2,k) - src(2,:);
    r =  sqrt(rx.*rx + ry.*ry);

  % compute greens function (natural logarithm in 2D)
    g = -1/2/pi * log (r);

  % correct for singularities (r=0->log=inf)
    idx = find (g==inf | g==-inf);
    g(idx)=0;

    u(k) = sum ( g.*densities );
  end
  
end


%/* ************************************************** */
function selftest()

n              = 2^6;    % number of particles
maxPntsPerNode = 10;      % points per box
maxLevel       = 20;     % maximum tree depth
verbose        = false;  % debugging
dim = 2;
points = rand(dim,n);  
densities = rand(1,n)/n;

[u,tree] = nbody(points,densities,maxPntsPerNode,maxLevel,false);
uex=direct(points(:,1:10), points, densities);
error = norm(u(1:10)-uex)/norm(uex)
tree.plottree();

end


