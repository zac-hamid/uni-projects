clc; clear all; close all;
O = [2000; 2500];
D = [1500; 2000; 1000];

r = [1, 2];
c = [1, 2, 3];

% at the beginning the table contains all 0 for x
x = zeros(size(r,1),size(c,1));

% continue until there is only 1 cell remaining
while size(r) + size(c) > 1,
i = r(1); j = c(1); 
m = min(O(i), D(j));
x(i,j) = m;
disp(x)

if m == O(i),
   % Need to remove row i from further consideration
   D(j) = D(j) - O(i);
   O(i) = 0;
   r(1) = [];
elseif m == D(j),
   % Need to remove column j from further consideration
   O(i) = O(i) - D(j);
   D(j) = 0;
   c(1) = [];
elseif O(i) == D(j),
   O(i) = 0;
   D(j) = 0;
   % Need to remove both row i and column j from further consideration
   r(1) = []; c(1) = [];
end
end
disp(x)
