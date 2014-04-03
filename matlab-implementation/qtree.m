classdef qtree < handle
% implements a quatree data structure
%    point based construction in 2D.
%    preorder and postorder traversals
%      
% see "doc qtree" for documentation on methods and classes
% see "nbody.m" for the 

  
%/* ************************************************** */
properties
  gids    % global ids 
  kids    % kid structure  kids.m (size) kids.ptr{i}  pointeres
  parent  % parent of the node
  level   % level of the node 
  data    % averaging-related data
  isleaf  % true or false
  anchor  % [x;y] coordinates of lower left point of a node
end

methods
%/* ************************************************** */
function this=qtree(parent,level,anchor)
% function this=qtree(parent,level,anchor)
% constructor for qtree 

if nargin<1, parent = [];           end;
if nargin<2, level  = 0;            end;
if nargin<3, anchor = [0,0];        end;

this.gids   = [];
this.kids   = [];
this.parent = parent;
this.level  = level;
this.data   = [];
this.isleaf = true;
this.anchor = anchor;

end


%/* ************************************************** */
function h=width(this)
% function h=width(this)
% get the width of the square that corresponds to this node
  h = 1/2^this.level;
end

%/* ************************************************** */
function [xmin,xmax,ymin,ymax]=corners(this)
% function [xmin,xmax,ymin,ymax]=corners(this)  
%  get the   coordinates of the corner points of a node
% lower left point [xmin; ymin], upper left point [xmin; ymax] etc. 
  
  h = width(this);
  xmin = this.anchor(1);
  ymin = this.anchor(2);

  xmax = xmin + h;
  ymax = ymin + h;

% to capture points on domain boundaries, modify xmax,ymax
  if xmax==1, xmax=1+eps; end 
  if ymax==1, ymax=1+eps; end;
end
  
  
%/* ************************************************** */
function create_kids(this)  
% function create_kids(this)    
% create the kid datastructure of this node

assert(isempty(this.kids),'Kids already created');

kidlevel= this.level+1;
kidh    = this.width/2;

% X Y
% 0 0 child  ( lower left  )
% 0 1 child  ( lower right )
% 1 0 child  ( upper left  )
% 1 1 child  ( upper right )
kids=[ [0 0]; [1 0]; [ 0 1]; [1 1] ];
anchor= zeros(4,2);
for i=1:4
  anchor(i,:) = this.anchor + kidh * kids(i, :);
  this.kids{i} = qtree(this, kidlevel,anchor(i,:));
end

%  update parent information
this.isleaf = false;
end

%/* ************************************************** */
% points = dim x num_of_points
function insert_points(this, gids, points, maxPntsPerNode,maxLevel)
% function insert_points(this, gids, points, maxPntsPerNode,maxLevel)  
% points         : dim x num_of_points array of the point coordinates
% gids           : ids of points (unsigned int)
% maxPntsPerNode : used to decide wether to split the node or not  
%
% This is the main construction routine for qtree. This will break if
% gids is not a permuation of 1:size(points,2)  
  
if length(gids)==0,return; end;

if this.isleaf
  nn = length(gids);
  no = length(this.gids);
  this.gids = unique( [this.gids(:); gids(:)] );

% this node is a leaf and the points fit here.
  if( no+nn <= maxPntsPerNode | this.level==maxLevel); return;  end

% this node will be split;
  create_kids(this);
  gids=this.gids;
  this.gids=[];
end

% now we insert the points to the kids
for k=1:4
  idx = this.kids{k}.points_in_node(points(:,gids));
  this.kids{k}.insert_points( gids(idx), points,...
                           maxPntsPerNode,maxLevel  );
end
end


%/* ************************************************** */
function idx = points_in_node(this, points)
% function idx = points_in_node(this, points)  
% idx : logical that has true for points within the node and
%
% the only complication is for points that lie right on 
% the boundaries;
  [xmin,xmax,ymin,ymax]=corners(this);

% now find the points that are in the box
  idx_x =  find(xmin <= points(1,:) & points(1,:) < xmax ) ;
  idx_y =  find(ymin <= points(2,:) & points(2,:) < ymax ) ;
  
  idx = intersect(idx_x, idx_y);
end

%/* ************************************************** */
function  preorder(this, visit, prune, user_data)
% function  preorder(this, visit, prune, user_data)  
% preorder traversal with prunning  
% visit(node,data)
% prune(node,data) can be empty
% user_data: user data structure for calculations

doPrune = false;
if ~isempty(prune),   doPrune= prune(this, user_data);  end
visit(this,user_data);
if ~this.isleaf & ~doPrune
   for k=1:4
     this.kids{k}.preorder( visit, prune, user_data);
   end
end
end


%/* ************************************************** */
function  postorder(this, visit, prune, user_data)
% function  postorder(this, visit, prune, user_data)  
% preorder traversal with prunning  
% visit(node,data)
% prune(node,data) can be empty
% user_data: user data structure for calculations

doPrune = false;
if ~isempty(prune),   doPrune= prune(this, user_data);  end
if ~this.isleaf & ~doPrune
   for k=1:4
     this.kids{k}.postorder( visit, prune, user_data);
   end
end
visit(this,user_data);
end

%/* ************************************************** */
function list=leaves(this)
% function list=leaves(this)  
% collects all the leaves in a single array  
  list={};
  cnt=0;
  function visit(this,dummy)
    if this.isleaf
      cnt=cnt+1;
      list{cnt}=this;
    end
  end
  this.preorder(@visit,[],[]);
end
  

%/* ************************************************** */
function print(this)
% function print(this)  
% print information about a node  
  print_node = @(this,dummy)...
      fprintf('node  at level %d: anchor:[%1.4f %1.4f]\n',...
              this.level,this.anchor(1),this.anchor(2));
  this.preorder(print_node,[],[]);
end

%/* ************************************************** */
function print_mids(this,leaves_only)
% function print_mids(this,leaves_only)  
% print the Morton IDs of nodes (using the class morton_id)
% leaves_only : if true, it only prints the leaves.
  mid = morton_id;
  if nargin<2, leaves_only=false; end;
  function print_node(this,dummy)
    if leaves_only & ~this.isleaf, return; end;
    fprintf('mid:');
    id = mid.id(this.level,this.anchor);
    mid.print(id);
    fprintf(': %20u at level %2d: anchor:[%1.4f %1.4f]\n',...
            id, this.level,this.anchor(1),this.anchor(2));

  end
  this.preorder(@print_node,[],[]);    
end

%/* ************************************************** */
function depth=find_depth(this)
% function depth=find_depth(this)  
% function depth=find_depth(this)  
% depth : depth of the tree.  
   depth = 0;
   function v=fdt(this,dummy)
      depth=max(this.level,depth);
   end
   this.preorder(@fdt,[],[]);
end

%/* ************************************************** */
function  plotnode(this,width) 
% function  plotnode(this,width)   
% plot the rectangul corresponding to the node  
  if nargin<2,width=1;end;
  rectangle('Curvature',[0.00,0.00],...
     'Position', [this.anchor(1),this.anchor(2),1/2^this.level,1/2^this.level], 'LineWidth',width);
end

%/* ************************************************** */
function plottree(this,markersize)
% function plottree(this,markersize)  
%  function plottree(this,markersize)
%  plots the whole tree.  
  hold on;
  if nargin<2,markersize=2;end;
  plotnode=@(node,dummy)node.plotnode(markersize);
  this.preorder(plotnode,[],[]);
  hold off;
end

end % methods
end % classdef
