function []=AntColony_MultiObj()

% AntColony;
AntColonyPath;
% WeightedSum;
% ParetoFront;


function []=AntColony()
% find shortest route from bottom to top of field

clc; clear all; close all; dbstop if error;

% generate nodes
R=20;% field size
fig=figure; axis([0 R+1 0 R+1]);
xlabel('x'); ylabel('y','rotation',0); hold on; grid on;
for y=1:R,% each up-down location
  for x=1:R,% each left-right location
    N(y,x,1)=x+(rand-0.5)/2;% random location
    N(y,x,2)=y+(rand-0.5)/2;
    plot(N(y,x,1),N(y,x,2),'b.');% show 
  end;
end;

% ant optimization iterations
Ph=zeros(R,R);% initially all zero pheromone
Ph_o=Ph;% copy of pheromone
T=50;% number of iterations
A=10;% number of ants
Ph_evap=0.05;% pheromone evaporation rate
for t=1:T,% iterations
  title(sprintf('Iteration %d',t));% show iterations
  for a=1:A,% each ant
    Ay=ceil(abs(randn)*R);% random start at any level below top
    while Ay>R,% out of the field
      Ay=ceil(abs(randn)*R);% re-generate location
    end;
    Ax=ceil(rand*(R-2)+1);% random start index, left-to-right
    AN=[]; AN=[N(Ay,Ax,1) N(Ay,Ax,2)];% location of the node
    plot(AN(1),AN(2),'r.');% show location
    for w=2:R,% start walking
      Ax_o=Ax; Ay_o=Ay;% copy locations
      Axn=Ax-1:Ax+1;% next feasible locations
      Axn(find(Axn==0))=[];% remove infeasible locations, too left
      Axn(find(Axn==R+1))=[];% too right
      Phn=Ph(Ay,Axn);% pheromone ahead of ant
      if min(Phn)==max(Phn),% all the same?
        ix=ceil(rand*length(Axn));% random next location index
      else
        [mx,ix]=max(Phn);% find max pheromone
      end;
      Ay=Ay+1; Ax=Axn(ix);% next move
      if Ay>R,% out of the field
        break;% stop
      end;
      if Ax<1, Ax=1; end;% cannot move too left
      if Ax>R, Ax=R; end;% cannot move too right
      AN(w,:)=[N(Ay,Ax,1) N(Ay,Ax,2)];% next location
      dx=AN(w,1)-AN(w-1,1);% distance travelled 
      dy=AN(w,2)-AN(w-1,2);
      ds=sqrt(dx^2+dy^2);
      Ph_o(Ay,Ax)=Ph_o(Ay,Ax)+1/(ds+eps);% lay pheromone
      Ph_o(Ay_o,Ax_o)=Ph_o(Ay_o,Ax_o)+1/(ds+eps);% lay pheromone
    end;
    plot(AN(:,1),AN(:,2),'r-'); drawnow;% show travelled path
  end;
  Ph=Ph*Ph_evap+Ph_o;% evaporate pheromone
end;
% final path
for Ay=1:R,% each level
  if Ay==1,% start from all possible x-location
    [mx,Ax]=max(Ph(Ay,:));% max pheromne
  else% not at start level
    Axn=[Ax-1:Ax+1];% allowed moves in x-direction
    Axn(find(Axn<1))=[]; Axn(find(Axn>R))=[];% not too left or too right
    [mx,ix]=max(Ph(Ay,Axn)); Ax=Axn(ix);% move to max pheromone
  end;
  Nn(Ay,:)=[N(Ay,Ax,1),N(Ay,Ax,2)];% store locations
  plot(N(Ay,Ax,1),N(Ay,Ax,2),'ko'); drawnow;% show ndes visited
end;
plot(Nn(:,1),Nn(:,2),'b-','linewidth',2);% overall path
% plot pheromone
figure; surf(Ph_o,'edgecolor','none');



function [f]=ObjFunc(x,y)
f=(1-x).^2+100*(y-x.^2).^2;


function []=AntColonyPath()
clc; clear all; close all; dbstop if error;
R=100; T=50;% range of aera, number of towns
Town=rand(2,T)*R;% locations of towns
fig=figure; plot(Town(1,:),Town(2,:),'b.');
hold on; grid on; axis([0 R 0 R]); drawnow;
for t=1:T,% label each town
  text(Town(1,t),Town(2,t),sprintf(' %d',t));
end; 
drawnow; hdLr=plot(0,0,'r-'); hdLb=plot(0,0,'b-'); % path handle
% distances between towns
for j=1:T,% each departing town
  for k=1:T,% each arriving town
    dx=Town(1,j)-Town(1,k);
    dy=Town(2,j)-Town(2,k);
    if j~=k,
      Dis(j,k)=sqrt(dx^2+dy^2);% distance between towns
      iDis(j,k)=1/(Dis(j,k)+eps);% inverse distance
    else
      iDis(j,k)=0;
    end;
  end;
end; 
iDis=iDis/sum(iDis(:));% normalize
% find starting point distance, target point distance
for t=1:T,
  S(t)=Town(1,t)^2+Town(2,t)^2;
end;
[mi,ix]=min(S); Start.Town=Town(:,ix); Start.ix=ix;
plot(Start.Town(1),Start.Town(2),'go','markerfacecolor','g');
[mi,ix]=max(S); Target.Town=Town(:,ix); Target.ix=ix;
plot(Target.Town(1),Target.Town(2),'ro','markerfacecolor','r');
% ACO parameters
ACO.G=100; ACO.N=200;% number of generation, ants 
% pheromone, inv distance, evaporation
ACO.a=0.995; ACO.b=(1-ACO.a); ACO.c=0.95;
ACO.Travel=realmax; ACO.Travel_=[];
ACO.Pher=ones(T,T)/T^2;% initial pheromone
% ACO main loop
for g=1:ACO.G,% each generation
  TravelAll=0; Visit=zeros(T,T);% travel per generation
  for n=1:ACO.N,% each ant
    Travel=0; cnt=2; Pt=[];% random start
    Z=rand(2,1)*R; Zdis=sqrt(sum((Town-repmat(Z,[1,T])).^2,1));
    [mi,ix]=sort(Zdis,'ascend'); Pt=ix(1:2); 
    Travel=Dis(Pt(end-1),Pt(end));
    while Pt(end)~=Start.ix & Pt(end)~=Target.ix,% path random start
      cnt=cnt+1; if cnt>T,% loop escape
        Travel=realmax; break;
      end;
      prob=(ACO.Pher(Pt(end),:).^ACO.a).*(iDis(Pt(end),:).^ACO.b); 
      prob(Pt)=0; [mx,ix]=max(prob);% movement probability
      Pt(end+1)=ix(1);% go along max probability
      Travel(end+1)=Dis(Pt(end-1),Pt(end));% distance travelled
    end;
    if Travel<realmax,% valid travel
      set(hdLr,'xdata',Town(1,Pt),'ydata',Town(2,Pt));
      title(sprintf('G=%d N=%d',g,n)); drawnow;
      for p=1:length(Pt)-1,% lay pheromone
        Visit(Pt(p),Pt(p+1))=Visit(Pt(p),Pt(p+1))+1;
      end;
      Visit=Visit/max(Travel);% normalize
    end;
  end;
  ACO.Pher=ACO.Pher+Visit;% increase pheromone
  ACO.Pher=ACO.Pher/sum(ACO.Pher(:));%normalize pheromone
  ACO.Pher=ACO.Pher.*ACO.c;% evaporate pheromone
  set(hdLr,'xdata',[],'ydata',[]); Pt=Start.ix;% show path found
  while Pt~=Target.ix,% better path for all ants
    prob=(ACO.Pher(Pt(end),:).^ACO.a).*(iDis(Pt(end),:).^ACO.b);
    prob(Pt)=0; [mx,ix]=max(prob); Pt(end+1)=ix;
    TravelAll=TravelAll+Dis(Pt(end-1),Pt(end));
  end;
  if TravelAll<ACO.Travel,% better path across generations
    ACO.Travel=TravelAll;
    set(hdLb,'xdata',Town(1,Pt),'ydata',Town(2,Pt));
  end;
  ACO.Travel_=[ACO.Travel_ ACO.Travel];% path record
  fprintf('Generation=%03d Travel=%5.3f\n',g,ACO.Travel);
end;
figure; plot(ACO.Travel_); grid on;
xlabel('Iterations'); ylabel('Total Travel');

% weighted sum approach
function []=WeightedSum()
clc; clear all; close all; dbstop if error;
[x,y]=meshgrid(-2:0.02:2,-2:0.02:2);% range
f1=x.^2+(y-1).^2;% obj function 1
f2=(x-1).^2+y.^2+2;% obj fnuction 2
f3=x.^2+(y+1).^2+1;% obj function 3
a=rand; b=rand; c=1-a-b;% weights
while c<0,% sum to 1, weights will change the objective
  a=rand; b=rand; c=1-a-b;
end;
f=a*f1+b*f2+c*f3;% weighted sum
figure; surfc(x,y,f,'edgecolor','none','facecolor','r'); hold on;
surf(x,y,f1,'edgecolor','none');% show
surf(x,y,f2,'edgecolor','none');
surf(x,y,f3,'edgecolor','none'); alpha(0.35);
[mi,ix]=min(f(:)); mi_x=x(ix); mi_y=y(ix); view(145,15);% find min value
plot3(mi_x,mi_y,mi,'ro','markersize',3,'linewidth',3);% show
title(sprintf('f=%5.3f x=%5.3f y=%5.3f\na=%5.3f b=%5.3f c=%5.3f',...
  mi,mi_x,mi_y,a,b,c));
xlabel('x'); ylabel('y'); zlabel('f','rotation',0);

% generate Pareto front, min problem
function []=ParetoFront()
clc; clear all; close all; dbstop if error;
N=500; rand('seed',100);
x=rand(1,N).^0.65*0.8+0.1;
y=rand(1,N).^0.65*0.8+0.1;
figure;
plot(x,y,'b.'); hold on; grid on; axis([0 1 0 1]);
for n=1:N,
  rnk(n)=sum(x(n)<x | y(n)<y);
end;
idx=find(rnk==max(rnk));
PF=[x(idx);y(idx)];
[sx,ix]=sort(PF(1,:),'ascend');
PF=PF(:,ix); plot(PF(1,:),PF(2,:),'r');
xlabel('f1'); ylabel('f2','rotation',0); 
title('Pareto Front - min problem');

% generate Pareto front, max problem, utility method
clc; clear all; close all; dbstop if error;
N=500; rand('seed',1000);% number of obj functions
x=rand(1,N).^1.5*0.65;% simulate random obj function f1
y=rand(1,N).^1.5*0.65;% simulate random obj function f2
figure;% new figure window, show obj functions
plot(x,y,'b.'); hold on; grid on; axis([0 1 0 1]);
for n=1:N,% Pareto opt condition
  rnk(n)=sum(x(n)>x | y(n)>y);
end;
idx=find(rnk==max(rnk));% find index
PF=[x(idx);y(idx)];% Pareto front
[sx,ix]=sort(PF(1,:),'ascend');% prepare plot Pareto front
PF=PF(:,ix); plot(PF(1,:),PF(2,:),'r');% plot
xlabel('f1'); ylabel('f2','rotation',0);% label
title('Pareto Front - max problem');% title
U=PF(1,:).*PF(2,:); [mxU,ix]=max(U);% utility U=f1*f2
xU=0:0.05:1; yU=mxU./xU; plot(xU,yU,'k');% plot utility curve
text(PF(1,ix),PF(2,ix),sprintf(' %5.3f',mxU));% utility found

