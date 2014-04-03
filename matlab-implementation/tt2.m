

n              = 2^6;    % number of particles
maxPntsPerNode = 1;      % points per box
maxLevel       = 20;     % maximum tree depth

point_type     = 'random' ;
%point_type     = 'regular';

dim = 2;
switch point_type
  case 'regular'
    l=log2(n)/2; %log4 logarithm
    h=2^-l;
    [x,y]=meshgrid(0:h:1-h,0:h:1-h);
    points = [x(:)'+h/2;y(:)'+h/2]; 
    n=size(points,2);
  otherwise % 'random' 
    points = rand(dim,n);   %using uniform distribution
end
densities = rand(1,n)/n;

[u,tree] = nbody(points,densities,maxPntsPerNode,maxLevel,false);


figure(2),tree.plottree(4); axis off; hold on;
plot(points(1,:),points(2,:),'or','MarkerSize',4);




