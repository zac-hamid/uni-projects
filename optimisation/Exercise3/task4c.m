clear all; clc; close all;
% Initial condition defined in problem
x0 = -10;

A = [0 1; 0 0];
B = [0; 1];

% Only want x from output of state space equation (not u)
C = eye(2);
D = [0; 0];

% q and Qu can be changed to examine effect on system response
q = 1;
Qx = [q^2 0; 0 0];
Qu = 100;


sys = ss(A,B,C,D);
[K,S,E] = lqr(sys, Qx, Qu);