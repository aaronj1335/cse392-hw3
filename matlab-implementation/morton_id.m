classdef morton_id < handle
% implements morton_id calculations for 2D points and quad_trees
% see "doc morton_id" and tt1.m for an example.
  
properties
  tb; % total number of bits
  lb; % bits to store level;
end

methods

%/* ************************************************** */
function o = morton_id()
% function o = morton_id()  
% assume uint64 for mid.
% lb: number of least signifinant bits that are reserved for level
% bits :  [1:lb]: level;  [lb+1 : 64]; interleaved x,y coordinates
  o.tb=64;
  o.lb=6;
end

%/* ************************************************** */
function mid = id(o, level,anchor)
% function mid = id(o, level,anchor)  
% mid   : the morton id of a quadtree node
% level : the level of the box
% anchor: the anchor (lower left point) of the box  
assert(o.tb == 64);  assert(o.lb < o.tb);

mid = uint64(0);
lb=o.lb; % level bits
cb=o.tb-lb; % coordinate bits
maxlevel = cb/2;

assert(level < bitset(uint8(0),lb+1,1) );
level_bits=bitget( uint8(level), [1:lb] );

ix = uint32 ( floor(  anchor(1)*2^maxlevel ) );
iy = uint32 ( floor(  anchor(2)*2^maxlevel ) );

cmax= bitset(uint32(0), maxlevel + 1, 1);
assert( ix<cmax & iy< cmax, ...
        'ax=%1.16E ix=%u, ay=%1.16E, iy=%u, cmax=%u\n', ...
         anchor(1), ix, anchor(2), iy,cmax); 


% set level bits (at the least significant places)
for i=1:lb
  mid=bitset( mid, i, level_bits(i));
end
% coordinates
for i=1:maxlevel
  mid=bitset(mid, lb+2*i-1, bitget(ix,i) );
  mid=bitset(mid, lb+2*i  , bitget(iy,i) );
end


end % function id

  
%/* ************************************************** */
function print(o,id)
% function print(o,id)  
% prints morton id. output bits are order as: MSB...LSB
% MSB: most significant bit
% LSB: least significant bit  
  bits = fliplr(bitget(id,1:o.tb)); % 
  cb = o.tb-o.lb;
  fprintf(' xy:');  for i=1:cb,      fprintf('%1d',bits(i));end
  fprintf(' lv:');  for i=cb+1:o.tb, fprintf('%1d',bits(i));end
end  



end % methods
end % class
