clear all; close all; clc;

syms x y z lambda;


m = 0.5*sqrt(5/3); n = 4*sqrt(5/3);
A = [   -z m m n;
        m -z m n;
        m m -z n;
        n n n 0]

d = det(A)

solve(d==0)


% B = [-z 20000 20000 1;
%     20000 -z 20000 1;
%     20000 20000 10000-z 1;
%     1 1 1 0];
% 
% 
% d = det(B)
% solve(d==0)

f=x*y*z;

% f = 8*x*y*z^2 - 200*(x+y+z);
% g = x+y+z-100;

g=2*(x*y+y*z+x*z)-10;

L = f+lambda*g

J = jacobian(L,[x,y,z,lambda])

s = solve(J(1),J(2),J(3),J(4))

s.lambda
s.x
s.y
s.z