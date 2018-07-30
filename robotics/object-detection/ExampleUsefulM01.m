% ---------------------------------------
% Example program, useful for solving ProjectM01, MTRN2500.S2.2016
% Questions: ask the Lecturer, via Moodle or by email
% (j.guivant@unsw.edu.au)

% ---------------------------------------
%e.g. run this way: 
%ExampleUsefulM01('.\data\HomeC001\');

function main(folder)
clc();
if ~exist('folder','var'),
    disp('YOU must specify the folder, where the files are located!');
    return;
end;

% load Depth and RGB images.
A = load([folder,'\PSLR_C01_120x160.mat']); CC=A.CC ; A=[];
A = load([folder,'\PSLR_D01_120x160.mat']); CR=A.CR ; A=[];

% length
L  = CR.N;

% Vertical coordinate of horizontal line of interest.
vL1=35;

% Some global variable, for being shared (you may use nested functions, 
% in place of using globals).

global CCC; 
CCC=[]; CCC.flagPause=0; 

%------------------
% We create the necessary plots/images/figures/etc.

% Create figure, where we will show Depth and RGB images.
figure(2); clf();

% subfigure, for Depth 
subplot(211) ; 
RR=CR.R(:,:,1);
hd = imagesc(RR);
ax=axis();
title('Depth');
colormap gray;
set(gca(),'xdir','reverse');

% In the same image, just show the line which was chosen.
hold on;
plot([ax(1),ax(2)],vL1+[0,0],'--r');

% In another subfigure, we show the associated RGB image.
subplot(212) ; hc = image(CC.C(:,:,:,1));
title('RGB');
set(gca(),'xdir','reverse');


% here we will show the depth associated to the selected line.
figure(3) ; clf();
hvl1 = plot(RR(vL1,:),'.');
ax=axis() ; ax(3:4) = [0,5000] ; axis(ax);
title(sprintf('Depth of Vertical Line #[%d]',vL1));
xlabel('Horizontal Pixels');
ylabel('Depth (mm)');
set(gca(),'xdir','reverse');

% .. another figure, for showing 3D points.
figure(4) ; clf() ; 
hp = plot3(0,0,0,'.','markersize',2) ; 
axis([0,3,-1.5,1.5,-0.4,0.9]);
title('3D');
xlabel('X (m)');
ylabel('Y (m)');
zlabel('Z (m)');
grid on;
rotate3d on ;


% some control button (you may define more, for other purposes)
uicontrol('Style','pushbutton','String','Pause/Cont.','Position',[10,1,80,20],'Callback',@MyCallBack1);


% Using "Helper API", just for this demo. (You need to implement those
% methods, for ProjectM01.)
A3D = API_Help3d() ;


% Periodic loop!
for i=1:L,
    if (CCC.flagPause), pause(0.3)  ; continue ; end;
    
    % Refresh RGB image, updating property 'cdata' of handle hc.
    set(hc,'cdata',CC.C(:,:,:,i));
    
    RR=CR.R(:,:,i);
    set(hd,'cdata',RR);

    r = RR(vL1,:);
    set(hvl1,'ydata',r);

    
    % "Processing"
    % obtain 3D points, for those pixels no faulty.
    iinz = find(RR>0);    %iinz=[];
    [xx,yy,zz]=A3D.ConvertSelectedDepthsTo3DPoints(single(RR)*0.001,iinz);
    [xx,zz]=A3D.Rotate2D(xx,zz,-10);
    
    % show then (update associated plot)
    set(hp,'xdata',xx,'ydata',yy,'zdata',zz);
    
    pause(0.1);     % freeze for about 0.1 second.
end;

end

% ---------------------------------------
% Callback function, I defined it and associated it to certain GUI button,
%(in this case, it is so basic, that we do not use its input arguments)
% BTW: we could have defined it as "function MyCallBack1(~,~)"  
function MyCallBack1(a,b)   
    global CCC;
    CCC.flagPause = ~CCC.flagPause; %Switch ON->OFF->ON -> and so on.
    return;    
end
% ---------------------------------------
% ---------------------------------------

% Example program, useful for solving ProjectM01, MTRN2500.S2.2016
% Questions: ask the Lecturer, via Moodle or by email
% (j.guivant@unsw.edu.au)

% ---------------------------------------
