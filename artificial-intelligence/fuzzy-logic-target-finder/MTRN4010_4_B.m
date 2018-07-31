% Author: Zachary Hamid
% Solution to MTRN4010 Project 4 - Part B, no virtual target

function []=MTRN4010_4_B()

clc; clear all; close all; dbstop if error;
set(0,'defaultaxesfontname','times new roman');

eval_vel=readfis('MTRN4010_vel.fis');
eval_ang=readfis('MTRN4010_ang3.fis');

field.range=50;
time.dt=1; time.T=500;

[fig]=FigureNew(field);
[car]=CarNew();
% Place new target at a random x, y and q
[target]=TargetNew( 2*(rand-0.5)*(field.range-field.range/6),...
                    2*(rand-0.5)*(field.range-field.range/6),...
                    2*(rand-0.5)*pi);

for t=0:time.dt:time.T,
  [car]=CarNow(car,time,evalfis(GetDistance(car,target),eval_vel),...
               evalfis(GetAngle(car,target),eval_ang));
  [car]=CarShow(car,t);
end;

% Get distance from the car to the target
function [d]=GetDistance(car,target)
dx = target.x - car.x;
dy = target.y - car.y;
dxy = sqrt(dx^2 + dy^2);
d=dxy;

% Get angle between car and target
function [da]=GetAngle(car,target)
dx = target.x - car.x;
dy = target.y - car.y;
da = atan2(dy,dx) - car.q;
% wrap to [-pi,pi]
if (da < -pi) da = da + 2*pi;
elseif (da > pi) da = da - 2*pi;
end

function [fig]=FigureNew(field)
fig=figure('units','normalized','position',[0.1 0.2 0.5 0.5]);
axis([-1 1 -1 1]*field.range); hold on; grid on; axis equal;
xlabel('x-direction'); ylabel('y-direction');

function [car]=CarNew()
car.x=0; car.y=0; car.q=0;
car.trace=[car.x; car.y; car.q];
car.shape=[ 2 0; 1 1; -1 1; -1 -1; 1 -1; 2 0]';
car.hdL.shape=plot(car.shape(1,:),car.shape(2,:),'color','b','linewidth',2);
car.hdL.trace=plot(car.trace(1,:),car.trace(2,:),'color',[0 0.66 0]);

function [car]=CarNow(car,time,v,w)
car.x=car.x+time.dt*v*cos(car.q);
car.y=car.y+time.dt*v*sin(car.q);
car.q=car.q+time.dt*w;
car.trace(:,end+1)=[car.x; car.y; car.q];

function [car]=CarShow(car,t)
ax=axis;
% 2D Rotation matrix
Rz=[  cos(car.q) -sin(car.q); 
      sin(car.q)  cos(car.q)];
shape=Rz*car.shape+repmat([car.x;car.y],1,6);
set(car.hdL.shape,'xdata',shape(1,:),'ydata',shape(2,:)); 
set(car.hdL.trace,'xdata',car.trace(1,:),'ydata',car.trace(2,:));
axis(ax); title(sprintf('Time %d',t)); pause(0.001);

% Creates a new car at a specific location (doesn't store trace since
% actual target is stationary)
function [target]=TargetNew(x,y,q)
target.x=x; target.y=y; target.q=q;
target.shape=[ 2 0; 1 1; -1 1; -1 -1; 1 -1; 2 0]';
target.hdL.shape=plot(target.shape(1,:),target.shape(2,:),'color','r','linewidth',2);
target=TargetShow(target);

% Plots the target, rotated correctly based on target's angle
% This is only called once since target doesn't move
function [target]=TargetShow(target)
Rz=[  cos(target.q) -sin(target.q); 
      sin(target.q)  cos(target.q)];
shape=Rz*target.shape+repmat([target.x;target.y],1,6);
set(target.hdL.shape,'xdata',shape(1,:),'ydata',shape(2,:)); 
