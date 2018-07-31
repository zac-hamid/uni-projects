
% Solution to Project 4 - Part C
function []=MTRN4010_4_C()

clc; clear all; close all; dbstop if error;
set(0,'defaultaxesfontname','times new roman');warning('off','all');

eval_vel=readfis('MTRN4010_vel.fis');
eval_ang=readfis('MTRN4010_ang4.fis');

field.range=50;
time.dt=1; time.T=1000;

[fig]=FigureNew(field);
PSO.DLB=0.01; PSO.DUB=2*field.range;    % V-target distance bounds
PSO.D=1; PSO.G=50; PSO.N=10;            % D = dimensions, G = generations, N = no. of particles
PSO.V=rand(PSO.D,PSO.N);                % Intial particle velocity for PSO
% Gbest => Best distance, gbest => lowest error
PSO.Gbest=[]; 
PSO.gbest=realmax;
PSO.Pbest=ones(1,PSO.N)*realmax;        % Same convention as above but for each particles personal best
PSO.pbest=ones(1,PSO.N)*realmax;
PSO.w1=0.9; PSO.w2=0.4;                 % commonly used time-decreasing inertia weight values
PSO.dw=PSO.w1-PSO.w2;                   % Initial and final inertia, used to balance local and gloabl search
PSO.cg=2; PSO.cp=2;                     % Social and cognitive factors
PSO.X=rand(PSO.D,PSO.N); PSO.BND=[PSO.DLB PSO.DUB]; %PSO distance bounds

PSO.X(1,:)=PSO.BND(1,1)+PSO.X(1,:)*diff(PSO.BND(1,:));  % Particles for v-target distance
gbest_hist=[];  % To store history of lowest error found

t_final = time.T;

[car]=CarNew();
[target]=TargetNew( 2*(rand-0.5)*(field.range-field.range/6),...
                    2*(rand-0.5)*(field.range-field.range/6),...
                    2*(rand-0.5)*pi);

carInit = car; % Remember where the car starts

for g=1:PSO.G,
    fprintf('Generation %d\n', g);
    disp(PSO.X);
    car=carInit;
    for n=1:PSO.N,
        d=PSO.X(1,n);
        [vtarget]=VTargetNew( target.x-d*cos(target.q), target.y-d*sin(target.q), target.q , false);
        % Run simulation for current particle n
        for t=0:time.dt:time.T,
            [car]=CarNow(car,time,evalfis(GetDistance(car,vtarget),eval_vel),...
                        evalfis(GetAngle(car,vtarget),eval_ang));
            [vtarget]=VTargetNow(vtarget,target,time,1.5*d/t_final); % Mark v-target arrive at target before end of time, so mobile robot can catch up
        end;
        dx_err = car.x-target.x;
        dy_err = car.y-target.y;
        da_err = car.q-target.q;
        % Wrap angle to [-pi,pi]
        while (da_err < -pi) da_err = da_err + 2 * pi; end;
        while (da_err > pi) da_err = da_err - 2 * pi; end;
        obj(n) = sqrt(dx_err^2 + dy_err^2 + da_err^2);
        
        % If this is now better than global best, make this global best
        if (obj(n) < PSO.gbest)
           PSO.gbest=obj(n);
           PSO.Gbest=PSO.X(:,n);
        end
        
        % If this is better than current particle best, make this current
        % particle best
        if (obj(n) < PSO.pbest(n))
           PSO.pbest(n) = obj(n);
           PSO.PBest(:,n) = PSO.X(:,n);
        end
        % Display best current virtual distance
        
        w=PSO.w2+(1-g/PSO.G)*PSO.dw; % Decrease the inertia weight (time decreasing strategy)
                                     % This helps to increase performance
        
        % Update distance (and particle velocity) using linearly decreasing
        % inertia weight strategy
        PSO.V=w*rand(PSO.D,PSO.N).*PSO.V+...
                PSO.cp*rand(PSO.D,PSO.N).*(PSO.Pbest-PSO.X)+...                 % Cognition part
                PSO.cg*rand(PSO.D,PSO.N).*(repmat(PSO.Gbest,[1,PSO.N])-PSO.X);  % Social part
        PSO.X=PSO.X+PSO.V;
        
        % Repair particles that are out of range (Out of the bounds of the
        % search)
        for d=1:PSO.D,
            z=find(PSO.X(d,:)<PSO.BND(d,1));
            PSO.X(d,z)=PSO.BND(d,1)+rand(1,length(z))*diff(PSO.BND(d,:));
            z=find(PSO.X(d,:)>PSO.BND(d,2));
            PSO.X(d,z)=PSO.BND(d,1)+rand(1,length(z))*diff(PSO.BND(d,:));
        end;
    end;
    fprintf('Generation %d Gbest: %5.3f gbest: %5.3f\n',...
                    g,PSO.Gbest,PSO.gbest);
    gbest_hist(g)=PSO.gbest;
end;
% Run final simulation of best solution found for virtual target distance
d=PSO.Gbest;
car=carInit;
[vtarget]=VTargetNew( target.x-d*cos(target.q), target.y-d*sin(target.q), target.q , true);

for t=0:time.dt:time.T,
  [car]=CarNow(car,time,evalfis(GetDistance(car,vtarget),eval_vel),...
               evalfis(GetAngle(car,vtarget),eval_ang));
  [car]=CarShow(car,t);
  [vtarget]=VTargetNow(vtarget,target,time,1.5*d/t_final); % Mark v-target arrive at target before end of time, so mobile robot can catch up
  [vtarget]=VTargetShow(vtarget);
end;

fig2=figure('units', 'normalized');
plot(1:PSO.G, gbest_hist);
xlabel('Generations');
ylabel('Objective Function Error');
title('Time history of fitness value');

function [d]=GetDistance(car,target)
dx = target.x - car.x;
dy = target.y - car.y;
dxy = sqrt(dx^2 + dy^2);
d=dxy;

function [da]=GetAngle(car,target)
dx = target.x - car.x;
dy = target.y - car.y;
da = atan2(dy,dx) - car.q;
% wrap to [-pi,pi]
while (da < -pi) da = da + 2*pi; end
while (da > pi) da = da - 2*pi; end

function [fig]=FigureNew(field)
fig=figure('units','normalized','position',[0.1 0.2 0.5 0.5]);
axis([-1 1 -1 1]*field.range); hold on; grid on; axis equal;
xlabel('x-direction'); ylabel('y-direction');

function [car]=CarNew()
car.x=0; 
car.y=0;
car.q=0;
car.trace=[car.x; car.y; car.q];
car.shape=[ 2 0; 1 1; -1 1; -1 -1; 1 -1; 2 0]';
car.hdL.shape=plot(car.shape(1,:),car.shape(2,:),'color','b','linewidth',2);
car.hdL.trace=plot(car.trace(1,:),car.trace(2,:),'color',[0 0 0.66]);

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

function [vtarget]=VTargetNew(x,y,q,visual)
vtarget.x=x; vtarget.y=y; vtarget.q=q;
vtarget.trace=[vtarget.x; vtarget.y; vtarget.q];
vtarget.shape=[ 2 0; 1 1; -1 1; -1 -1; 1 -1; 2 0]';
if(visual==true) vtarget.hdL.shape=plot(vtarget.shape(1,:),vtarget.shape(2,:),'color','g','linewidth',2); end;
if(visual==true) vtarget.hdL.trace=plot(vtarget.trace(1,:),vtarget.trace(2,:),'color',[0 0.66 0]); end;

function [vtarget]=VTargetNow(vtarget,target,time,v)
if (abs(vtarget.x - target.x) > 0.01) vtarget.x=vtarget.x+time.dt*v*cos(vtarget.q); end;
if (abs(vtarget.y - target.y) > 0.01) vtarget.y=vtarget.y+time.dt*v*sin(vtarget.q); end;
vtarget.trace(:,end+1)=[vtarget.x; vtarget.y; vtarget.q];

function [vtarget]=VTargetShow(vtarget)
% 2D Rotation matrix
Rz=[  cos(vtarget.q) -sin(vtarget.q); 
      sin(vtarget.q)  cos(vtarget.q)];
shape=Rz*vtarget.shape+repmat([vtarget.x;vtarget.y],1,6);
set(vtarget.hdL.shape,'xdata',shape(1,:),'ydata',shape(2,:)); 
set(vtarget.hdL.trace,'xdata',vtarget.trace(1,:),'ydata',vtarget.trace(2,:));