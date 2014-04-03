clear all; clear globals; dim=2;  % constants  and preamble

% RUN PARAMETERS
n              = 2^4;    % number of particles
maxPntsPerNode = 1;      % points per box
maxLevel       = 20;     % maximum tree depth
verbose        = true;

%     point construction
pnts  = rand(dim,n); 
%pnts = (0.6+ randn(dim, n)*.01).^2;
%     pathological cases
%onepnt  =[0;1]; pnts=repmat(onepnt,1,n); 


% MAIN SCRIPT
gids = 1:n;
o = qtree;
o.insert_points(gids,pnts,maxPntsPerNode,maxLevel);
o.plottree;
axis off; hold on;
plot(pnts(1,:),pnts(2,:),'or','MarkerSize',5);
axis off;

if verbose
   % print morton ids, all nodes
   disp(' all nodes');
   o.print_mids;
   % print morton ids, leaves only
   disp('  leaves only');
   o.print_mids(true);
end
depth=find_depth(o);
fprintf('tree depth is %d\n', depth);


