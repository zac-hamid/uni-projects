function []=ParticleSwarm()

% PSO implementation -1
% PSO1;

% PSO implementation -2
PSO2;

function []=PSO1()
clc; clear all; close all; dbstop if error;% program management
% Michaelwicz Function f*=-1.801 at [2.20319,1.57049]
fstr='-sin(x)*(sin(x^2/pi))^20-sin(y)*(sin(2*y^2/pi))^20';
% Converting to an inline function and vectorization
f=vectorize(inline(fstr));

% PSO parameter
Num_iterations=50; n=20; 
% range= [xmin xmax ymin ymax] ;
range=[0 4 0 4];
% Setting the parameters: alpha, beta
alpha=0.2; beta=0.5;
% Grid values of the objective for visualization only
Ndiv=100;
dx=(range(2)-range(1))/Ndiv;dy=(range(4)-range(3))/Ndiv;
xgrid=range(1):dx:range(2); ygrid=range(3):dy:range(4);
[x,y]=meshgrid(xgrid,ygrid);
z=f(x,y);
% Display the shape of the function to be optimized
figure(1); surfc(x,y,z);
best=zeros(Num_iterations,3); % initialize history

% Start Particle Swarm Optimization
% generating the initial locations of n particles
[xn,yn]=init_pso(n,range);
% Display the particle paths and contour of the function
figure(2); Fit=realmax; Fit_=[];
% Start iterations
for i=1:Num_iterations,
  % Show the contour of the function
  contour(x,y,z,15); hold on;
  % Find the current best location (xo,yo)
  zn=f(xn,yn);
  zn_min=min(zn);
  xo=min(xn(zn==zn_min));
  yo=min(yn(zn==zn_min));
  zo=min(zn(zn==zn_min));
  if zn_min<Fit,
    Fit=zn_min;
  end;
  Fit_=[Fit_ Fit];
  % Trace the paths of all roaming particles
  % Display these roaming particles
  plot(xn,yn,'.',xo,yo,'*'); axis(range); 
  pause((Num_iterations-i)*0.02);
  % Move all the particles to new locations
  [xn,yn]=pso_move(xn,yn,xo,yo,alpha,beta,range);
  % Use "hold on" to display paths of particles
  hold off;
  % History
  best(i,1)=xo; best(i,2)=yo; best(i,3)=zo;
end % end of iterations
figure; plot(Fit_); grid on; 
xlabel('Generations'); ylabel('Fitness');

% All subfunctions are listed here

% Intial locations of n particles
function [xn,yn]=init_pso(n,range)
xrange=range(2)-range(1); yrange=range(4)-range(3);
xn=rand(1,n)*xrange+range(1);
yn=rand(1,n)*yrange+range(3);

% Move all the particles toward (xo.yo)
function [xn,yn]=pso_move(xn,yn,xo,yo,a,b,range)
nn=size(yn,2); %a=alpha, b=beta
xn=xn.*(1-b)+xo.*b+a.*(rand(1,nn)-0.5);
yn=yn.*(1-b)+yo.*b+a.*(rand(1,nn)-0.5);
[xn,yn]=findrange(xn,yn,range);

% Make sure the particles are within the range
function [xn,yn]=findrange(xn,yn,range)
nn=length(yn);
for i=1:nn,
  if xn(i)<=range(1), xn(i)=range(l); end
  if xn(i)>=range(2), xn(i)=range(2); end
  if yn(i)<=range(3), yn(i)=range(3); end
  if yn(i)>=range(4), yn(i)=range(4); end
end


function []=PSO2()
clc; clear all; close all; dbstop if error;
% alternative PSO implementation

% find minimum of objective function
R=[4]; R=[0 1 0 1]*R;% range
[x,y]=meshgrid(R(1):0.05:R(2),R(3):0.05:R(4));% mesh grid points
f=ObjFunc(x,y);% objective function values
fig=figure; surfc(x,y,f,'edgecolor','none');% show objective surface surface
hold on; grid on; alpha(0.5); xlabel('x'); ylabel('y'); zlabel('f','rotation',0);

% PSO parameters
PSO.G=30; PSO.N=50;% generation, particles
PSO.w=0.6; PSO.r1=1.5; PSO.r2=1.5;% inertial, random gain factos
PSO.V=zeros(2,PSO.N);% particle velocity
PSO.Gbest=[]; PSO.gbest=realmax;% group best particle, value
PSO.Pbest=[]; PSO.pbest=ones(1,PSO.N)*realmax;% particle best, value
for n=1:PSO.N,% generate particles
  PSO.X(:,n)=NewParticle(R);
end;
f=ObjFunc(PSO.X(1,:),PSO.X(2,:));% show objective function values
hdL=plot3(PSO.X(1,:),PSO.X(2,:),f,'r.');% on surface
hdL0=plot3(0,0,0,'b*'); Fit_=[];

% PSO process
for g=1:PSO.G,% each generation
  for n=1:PSO.N,% each particle
    f(g,n)=ObjFunc(PSO.X(1,n),PSO.X(2,n));% objective function values
    if f(g,n)<PSO.gbest,% check global best particle
      PSO.gbest=f(g,n);% update value
      PSO.Gbest=PSO.X(:,n);% update group best particle
      title(sprintf('f=%5.3f X= %5.3f %5.3f',PSO.gbest,PSO.Gbest));% show
    end;
    if f(g,n)<PSO.pbest(n),% check particle best history
      PSO.pbest(n)=f(g,n);% update particle value
      PSO.Pbest(:,n)=PSO.X(:,n);% update particle
    end;
  end;
  Fit_=[Fit_ PSO.gbest];
  % show particles
  set(hdL,'xdata',PSO.X(1,:),'ydata',PSO.X(2,:),'zdata',f(g,:));
  set(hdL0,'xdata',PSO.Gbest(1),'ydata',PSO.Gbest(2),'zdata',PSO.gbest);
  drawnow; pause(0.2);
  % PSO update
  PSO.V=PSO.w*PSO.V+...
    PSO.r1*rand(2,PSO.N).*(repmat(PSO.Gbest,1,PSO.N)-PSO.X)+...
    PSO.r2*rand(2,PSO.N).*(PSO.Pbest-PSO.X);% update particle velocity
  PSO.X=PSO.X+PSO.V;% update particle position
  % PSO check range
  for n=1:PSO.N,% each particle
    mi1=min(PSO.X(1,n)); mx1=max(PSO.X(1,n));% min, max values
    mi2=min(PSO.X(2,n)); mx2=max(PSO.X(2,n));
    if (mi1<R(1) | mx1>R(2) | mi2<R(3) | mx2>R(4)),% out of range
      PSO.X(:,n)=NewParticle(R);% regenerate new particle
    end;
  end;  
end;
figure; stairs(Fit_); grid on;
xlabel('Generations'); ylabel('Fitness');

% calculate objective function
function [f]=ObjFunc(x,y)
% f=-sin(x).*(sin(x.^2./pi)).^20-sin(y).*(sin(2.*y.^2./pi)).^20;
x=x-1; y=y-1; f=(4-2.1.*x.^2+x.^4/3).*x.^2+x.*y+4.*(y.^2-1).*y.^2;

% generate new particle
function [X]=NewParticle(R)
X=rand(2,1);% uniform random value
X(1,:)=(X(1,:)-R(1))*R(2);% align within ranges
X(2,:)=(X(2,:)-R(3))*R(4);
