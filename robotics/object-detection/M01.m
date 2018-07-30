function M01(folder)
clc();
    if ~exist('folder','var'),
        disp('Folder could not be located!');
        return;
    end;
    
% load Depth and RGB images.
A = load([folder,'\PSLR_C01_120x160.mat']); CC=A.CC ; A=[];
A = load([folder,'\PSLR_D01_120x160.mat']); CR=A.CR ; A=[];

f = figure(1);

vL1=35;

subplot(211) ; 
RR=CR.R(:,:,1);
hd = imagesc(RR);
ax=axis();
title('Depth');
colormap gray;
set(gca(), 'xdir', 'reverse');

hold on;
plot([ax(1),ax(2)],vL1+[0,0],'--r');

subplot(212);
hc = image(CC.C(:,:,:,1));
title('RGB');
set(gca(), 'xdir', 'reverse');

f2=figure(2);
clf();
hvl1 = plot(RR(vL1,:),'.');
hold on;
hvl2 = plot(zeros(1,length(RR(vL1,:))),'r+');
set(hvl2, 'Xdata', [], 'Ydata', []);
ax=axis(); 
ax(3:4) = [0,5000]; 
axis(ax);
title(sprintf('Depth of Vertical Line #[%d]',vL1));
xlabel('Horizontal Pixels');
ylabel('Depth (mm)');

f3=figure(3);
clf();
hp = plot3(0,0,0,'.','markersize',2);
hold on;
r = plot3(0,0,0, '.r');
axis([0,3,-1.5,1.5,-0.4,0.9]);
title('3D');
xlabel('X (m)');
ylabel('Y (m)');
zlabel('Z (m)');
grid on;
rotate3d on;


endBut = uicontrol('Style', 'pushbutton', ...
                   'String', 'Stop simulation', ...
                   'Position', [5, 0, 100, 20], ...
                   'Callback', @endSimulation);
pauseBut = uicontrol('Style', 'checkbox', ...
                   'String', 'Pause', ...
                   'Position', [120, 0, 60, 20], ...
                   'Callback', @pauseSimulation);
               
riskyCB = uicontrol('Style', 'checkbox', ...
                   'String', 'Show risky points', ...
                   'Position', [180, 0, 120, 20], ...
                   'Callback', @riskyPointSwitch);
correctionCB = uicontrol('Style', 'checkbox', ...
                   'String', 'Rotate points', ...
                   'Position', [300, 0, 100, 20], ...
                   'Callback', @rotatePointSwitch);

               
A3D = API_Help3d();

set(f, 'WindowStyle', 'Docked');
set(f2, 'WindowStyle', 'Docked');
set(f3, 'WindowStyle', 'Docked');

L = CC.N;
simulationEnded = false;
simulationPaused = false;
showRiskyPoints = false;
correctPoints = false;

for i=1:L,
    % If trying to close the figure, don't reopen
    if (~ishandle(f) || ~ishandle(f2) || ~ishandle(f3)),
        break
    end
    if simulationEnded, 
        break
    end
    set(hc,'cdata',CC.C(:,:,:,i));
    RR=CR.R(:,:,i);
    set(hd,'cdata',RR);
    
    
    [xx,yy,zz]=depthTo3DPoint(RR);
    
    if (correctPoints),
        [xx,zz]=rotate3DPoint(xx,zz, 10);
    end
    
    uselessPoints = find(zz > 1 | zz < -0.05);
    
    xx(uselessPoints) = [];
    yy(uselessPoints) = [];
    zz(uselessPoints) = [];
    
    faultyPoints = find(xx <= 0);
    
    xx(faultyPoints) = [];
    yy(faultyPoints) = [];
    zz(faultyPoints) = [];
    
    if (showRiskyPoints),
        riskyPoints = find(xx < 0.6 & zz > 0.15);
        xx_risky = xx(riskyPoints);
        yy_risky = yy(riskyPoints);
        zz_risky = zz(riskyPoints);
    else
        xx_risky = [];
        yy_risky = [];
        zz_risky = [];
    end
    
    set(hp,'xdata',xx,'ydata',yy,'zdata',zz);
    
    set(r,'xdata',xx_risky,'ydata',yy_risky,'zdata',zz_risky);
    
    ind = find(RR(vL1,:) == 0);
    RR(vL1, ind) = 10000;
    set(hvl1,'ydata',RR(vL1,:));

    xx_dif = getSalientSegments(150);
    
    xx_dif(3) = 10000;
    
    set(hvl2,'ydata',xx_dif);
    pause(0.1);
end;


    function endSimulation(~,~)
        simulationEnded = ~simulationEnded;
        return;
    end

    function riskyPointSwitch(~,~)
        showRiskyPoints = ~showRiskyPoints;
        return;
    end

    function rotatePointSwitch(~,~)
        correctPoints = ~correctPoints;
        return;
    end

    function pauseSimulation(~,~)
        simulationPaused = ~simulationPaused;
        if (simulationPaused),
            uiwait();
        else
            uiresume();
        end
        return;
    end

    function [xx,yy,zz] = depthTo3DPoint(depthData)
        xx=double(depthData)*diag(1);
        
        vy=linspace((1-80)*5/594,(160-80)*5/594,160);
        Ap=diag (vy);
        yy=double(RR)*Ap;
        vz=linspace((1-60)*4/592,(120-60)*4/592,120);
        B=diag(vz);
        
        zz=-B*double(depthData);
        xx=xx/1000;
        yy=yy/1000;
        zz=zz/1000;
        xx=xx(:).';
        yy=yy(:).';
        zz=zz(:).';
        
    end

    function [xx,zz] = rotate3DPoint(x, z, theta)
        dd = sqrt(power(x,2) + power(z,2));
        angle = radtodeg(atan2(z,x));
        phi = angle - theta;
        xx = dd.*cos(degtorad(phi));
        zz = dd.*sin(degtorad(phi));
        zz = zz + 0.2;
    end

    function[xx_dif] = getSalientSegments(dist)
    xx_dif = zeros(1,length(RR(vL1,:)));
    d = diff(double(RR(vL1,:)));
    l = find(d>150|d<-150);
    
    for j=1:length(l),
       if (RR(vL1, l(j)) == 10000),
        continue;
       elseif (RR(vL1, l(j)+1) == 10000),
           if (j+1 <= length(l) && l(j+1)+1 <= length(RR(vL1,:))),
            dif = double(RR(vL1, l(j+1)+1)) - double(RR(vL1, l(j)));
            if (dif > dist),
               xx_dif(l(j)) = RR(vL1,l(j));
               if (j-1 > 0),
                xx_dif(l(j-1)+1) = RR(vL1,l(j-1)+1);
               end
            elseif (dif < -dist),
               xx_dif(l(j+1)+1) = RR(vL1,l(j+1)+1);
               if (j+2 <= length(l) && l(j+2) <= length(RR(vL1,:))),
                   xx_dif(l(j+2)) = RR(vL1,l(j+2));
               end
            end
           end
       else
           dif = double(RR(vL1, l(j)+1)) - double(RR(vL1, l(j)));
           if (dif > dist),
              xx_dif(l(j)) = RR(vL1,l(j));
              if (j-1 > 0),
                 xx_dif(l(j-1)+1) = RR(vL1,l(j-1)+1); 
              end
           elseif (dif < -dist),
               xx_dif(l(j)+1) = RR(vL1,l(j)+1);
               if (j+1 <= length(l) && l(j+1) <= length(RR(vL1,:))),
                  xx_dif(l(j+1)) = RR(vL1, l(j+1)); 
               end
           end
       end
    end

    xx_dif(xx_dif == 0) = 10000;
    end
end