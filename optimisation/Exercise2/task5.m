clear all; close all; clc;
% Design constraints
d_min = 2;
d_max = 14;

t_min = 0.2;
t_max = 0.8;

% Number of grid points in either dimension
N = 200;

d_step = (d_max - d_min)/(N-1);
t_step = (t_max - t_min)/(N-1);

d_vec = [d_min:d_step:d_max];
t_vec = [t_min:t_step:t_max];

[x,y] = meshgrid(d_vec, t_vec);

% Plot objective function surface with contour
f = ObjFunction(x,y);
surfc(x,y,f,'LineStyle','none', 'FaceAlpha',0.5);
xlabel('x_{1}'); ylabel('x_{2}'); zlabel('f');


% Constraints:
% g1(d,t) = InducedStress(d,t) - 500 <= 0
% g2(d,t) = InducedStress(d,t) - BucklingStress(d,t) <= 0
% 2 <= d <= 14; 0.2 <= t <= 0.8

g1 = InducedStress(x,y) - 500;
g2 = InducedStress(x,y) - BucklingStress(x,y);

i1 = find(g1 > 0);
i2 = find(g2 > 0);

x(i1) = NaN;
x(i2) = NaN;
y(i1) = NaN;
y(i2) = NaN;

figure;
% Plot objective function surface with contour
f2 = ObjFunction(x,y);
surfc(x,y,f2,'LineStyle','none', 'FaceAlpha',0.5);
xlabel('x_{1}'); ylabel('x_{2}'); zlabel('f');

[f_min, index] = min(f2,[],'all','omitnan','linear');
[row,col] = ind2sub([N, N],index);

x_min = x(row,col);
y_min = y(row,col);

hold on; plot3(x_min,y_min,f_min,'ro');
text(x_min,y_min,f_min,sprintf('[%.2f, %.2f] = %.2f', x_min, y_min, f_min));
title(sprintf('x_{1} = %.4f, x_{2} = %.4f, f = %.4f',x_min,y_min,f_min));

% Formula for induced stress
function [si]=InducedStress(d,t)
si = 2500./(pi.*d.*t);
end

% Formula for buckling stress
function [sb]=BucklingStress(d,t)
sb = (pi.^2.*(0.85.*10.^6).*(d.^2+t.^2))./(8.*250^2);
end

% Objective function to be optimized
function [f]=ObjFunction(d,t)
f = 9.82.*d.*t + 2.*d;
end