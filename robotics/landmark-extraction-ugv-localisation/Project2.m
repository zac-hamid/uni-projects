% Author: Zachary Hamid, Z5059915
% Program: Solution for AAS, S1 2018, Project2, Parts 1-4
function Project2(IMUFile, VelFile, LaserFile)
    close; 
    clear all;
    if ~exist('IMUFile','var'), IMUFile ='IMU_dataC.mat'; end;
    if ~exist('VelFile','var'), VelFile ='Speed_dataC.mat'; end;
    if ~exist('LaserFile','var'), LaserFile ='Laser__2C.mat'; end;

    % load data files
    load(IMUFile);
    load(VelFile);
    load(LaserFile);
    
    figure(1) ; clf(); hold on;
    
    % GUI Handles
    MyGUIHandles.nonHRHandle = plot(0,0,'b.');
    MyGUIHandles.hrHandle = plot(0,0,'r+');
    
    axis([-10,10,-10,10]);                                % focuses plot on this region ( of interest in L220)
    MyGUIHandles.OOIHandle = plot(0,0,'co');
    MyGUIHandles.PoleHandle = plot(0,0,'go', 'markers', 10);
    MyGUIHandles.titleHandle = title('');
    grid on;
    xlabel('X(m)');
    ylabel('Y(m)');
    
    figure(2); clf(); hold on;
    MyGUIHandles.posHandle = plot(0,0,'o','color','magenta');
    MyGUIHandles.pathHandle = plot(0,0,'-','color','green');
    MyGUIHandles.initialOOIHandle = plot(0,0,'o', 'markers', 8, 'color', 'black');
    MyGUIHandles.currentOOIHandle = plot(0,0,'o', 'markers', 8, 'color', 'cyan');
    MyGUIHandles.currentOOITextHandles = [text(0,0,'', 'Color', 'cyan'), text(0,0,'', 'Color', 'cyan'), text(0,0,'', 'Color', 'cyan'), text(0,0,'', 'Color', 'cyan'), text(0,0,'', 'Color', 'cyan')];
    grid on;
    axis([-10,10,-1,10]);
    xlabel('X(m)');
    ylabel('Y(m)');
    legend('Current UGV Position', 'UGV Path', 'Initial OOIs', 'Current OOIs');
    
    figure(3); clf();
    subplot(3,1,1);
    MyGUIHandles.wzHandle = plot(0,0);
    grid on;
    ylabel('wz (degrees / s)');
    xlabel('Time (s)');
    xlim([0 223]);

    subplot(3,1,2);
    MyGUIHandles.yawHandle = plot(0,0);
    grid on;
    xlabel('Time (s)');
    ylabel('Yaw (degrees)');
    xlim([0 223]);

    subplot(3,1,3);
    MyGUIHandles.speedHandle = plot(0,0);
    grid on;
    xlabel('Time (s)');
    ylabel('Speed (m/s)');
    xlim([0 223]);
    
    scanN = dataL.N;
    
    % Bias in yaw was found by average of first 1 second of data
    yaw_bias = -0.0170;

    % Convert time to double
    time = double(IMU.times);
    laserTime = double(dataL.times);
    
    % Offset to 0 and get as seconds
    time = (time-time(1))/10e+03;
    laserTime = (laserTime-laserTime(1))/10e+03;

    N = length(time);
    
    % Array to store path of UGV
    posArray = zeros(2,N);
    posArrayIndex = 1;
    
    wz = IMU.DATAf(6,:);

    % remove bias in the yaw measurements
    wz = wz - yaw_bias;

    dt = 0.005;
    
    % Initial conditions, x = 0, y = 0, theta = 90 deg
    X = zeros(3,N);
    X(3) = 90;
    laserIndex = 1;
    
    for i = 1:(N-1)
        angularRate = wz(i);
        speeds = Vel.speeds(i);
        
        X(:,i+1) = ProcessDeadReckoning(X(:,i), angularRate, speeds, dt);
        
        % Update UGV path array
        posArray(1:2,posArrayIndex) = [X(1,i+1);X(2,i+1)];
       
        if (laserIndex <= length(laserTime))
            if (time(i) >= laserTime(laserIndex))
                % Only update plot everytime laser data is available
                % (improves performance)
                set(MyGUIHandles.wzHandle, 'xdata', time(1:i), 'ydata', wz(1:i));
                set(MyGUIHandles.yawHandle, 'xdata', time(1:i), 'ydata', X(3,1:i));
                set(MyGUIHandles.speedHandle, 'xdata', time(1:i), 'ydata', Vel.speeds(1:i));
        
                set(MyGUIHandles.posHandle, 'xdata', X(1,i+1), 'ydata', X(2,i+1));
                set(MyGUIHandles.pathHandle, 'xdata', posArray(1,1:posArrayIndex),'ydata',posArray(2,1:posArrayIndex));
                
                % Laser data is updated
                scan_i = dataL.Scans(:,laserIndex);
                ProcessScan(scan_i, X(1, i+1), X(2,i+1), X(3,i+1), MyGUIHandles, laserIndex, scanN);
                laserIndex = laserIndex + 1;
                pause(0.01);    % wait for 10ms to plot
            end
        end
        posArrayIndex = posArrayIndex + 1;
    end

    return;
end

% Part A
% gyro = [ pitch, roll, yaw ] angular velocities
% Assumes bias is taken out of angularRate previously

function attitude = EstimateAttitude(prevHeading, angularRate, dt)
    % Assuming pitch and roll = 0
    wz = angularRate;

    %convert to degrees / second
    wz = rad2deg(wz);
    
    % convert to co-ordinate frame B
    wzB = -wz;
    
    attitude = prevHeading + dt*wzB;
    return;
end

% Part B
% X0 - Previous state of UGV
% angularRate - current angular speed
% dt - integration time

function X = ProcessDeadReckoning(X0, angularRate, speed, dt)
    X(3)        = EstimateAttitude(X0(3), angularRate, dt);
    thetarad    = deg2rad(X0(3));
    X(1)        = X0(1) + dt*(speed*cos(thetarad));
    X(2)        = X0(2) + dt*(speed*sin(thetarad));
return;
end

% Process laser data
% scan - actual laser data points
% posX/Y - position of UGV (for transformation of laser data)
% heading - heading of UGV (for transformation of laser data)
% mgh - GUI handles
% i - current laser scan
% N - number of points in scan

function ProcessScan(scan,posX, posY, heading,mgh,i,N)
    persistent initialOOIs
    % Extract range and intensity information, from raw measurements.
    % Each "pixel" is represented by its range and intensity of reflection.
    % It is a 16 bits number whose bits 0-12 define the distance (i.e. the range)
    % in cm (a number 0<=r<2^13), and bits 13-15 indicate the intensity 
    %( a number 0<=i<8 ).

    % We extract range and intensity, here.
    %useful masks, for dealing with bits.
    mask1FFF    = uint16(2^13-1);
    maskE000    = bitshift(uint16(7),13);

    intensities = bitand(scan,maskE000);

    ranges      = single(bitand(scan,mask1FFF))*0.01; 
    % Ranges expressed in meters, and unsing floating point format (e.g. single).

    % 2D points, expressed in Cartesian. From the sensor's perpective.
    angles = [0:360]'*0.5* pi/180 ;         % associated angle, for each individual range in a scan
    X = cos(angles).*ranges;
    Y = sin(angles).*ranges;
    
    [X, Y] = GlobalTransform(X,Y,posX,posY,heading);
    
    % Dynamic plotting
    s=sprintf('(Plotting...)\nShowing scan #[%d]/[%d]\r',i, N);  % Set title string
    set(mgh.nonHRHandle,'xdata',X,'ydata',Y);                    % plot x,y data
    set(mgh.titleHandle,'string',s);

    ii = find(intensities~=0);                                   % find those "pixels" that had intense reflection (>0) (aka: Highly Reflective pixels, HR)
    set(mgh.hrHandle, 'xdata', X(ii), 'ydata', Y(ii));           % plot highly reflective ones
    OOIs = ExtractOOIs(X, Y, intensities);
    brilliant = find(OOIs.Colors~=0);
    
    if (i == 1)
        if (OOIs.N > 0)
            OOIs.ID(1:OOIs.N) = 0;
            OOIs.ID(brilliant) = [1:length(OOIs.ID(brilliant))];
            initialOOIs = ExtractFromList(OOIs, brilliant);
            set(mgh.initialOOIHandle, 'xdata', OOIs.Centers(1,brilliant), 'ydata', OOIs.Centers(2,brilliant));
            
            str = cellstr(num2str(initialOOIs.ID(:)));
            
            % Initial OOI labels don't move, so can be set statically
            figure(2);
            text(initialOOIs.Centers(1,:)+0.3,initialOOIs.Centers(2,:)+0.1, str);
            
        end
    else
        OOIs.ID(1:OOIs.N) = 0;
        currentOOIs = ExtractFromList(OOIs, brilliant);
        currentOOIs = InferID(currentOOIs, initialOOIs);
        
        if(currentOOIs.N > 0)
            set(mgh.currentOOIHandle, 'xdata', currentOOIs.Centers(1,:), 'ydata', currentOOIs.Centers(2,:));
            str = cellstr(num2str(currentOOIs.ID(:)));
            
            % Set text handle of each current OOI object with it's inferred
            % ID
            for i = 1:currentOOIs.N
                set(mgh.currentOOITextHandles(i), 'Position', [currentOOIs.Centers(1,i)-0.5,currentOOIs.Centers(2,i)+0.3], 'String', str(i));
            end
            
            % Special case, if there are currentOOIs missing, clear the
            % strings of the missing OOIs
            if (currentOOIs.N < initialOOIs.N)
                for i = (currentOOIs.N+1):initialOOIs.N
                    set(mgh.currentOOITextHandles(i), 'String', '');
                end
            end
        else
            % If no current OOIs are present, clear all current OOI handles
            set(mgh.currentOOIHandle, 'xdata', [], 'ydata', []);
            for i = 1:initialOOIs.N
                set(mgh.currentOOITextHandles(i), 'String', '');
            end
        end
        PlotOOIs(OOIs, mgh);
    end
    return;
end

% Returns a list of OOIs from another list of OOIs based on an index range
% given
% OOIs - List to extract from
% indexList - Range to extract from
% Returns: OOIList - List of OOIs that were extracted

function OOIList = ExtractFromList(OOIs, indexList)
    OOIList.N = length(indexList);
    OOIList.Centers = OOIs.Centers(:,indexList);
    OOIList.Colors = OOIs.Colors(indexList);
    OOIList.Diameters = OOIs.Diameters(indexList);
    OOIList.ID = OOIs.ID(indexList);
end

% Transform a point in local frame to global frame
function [X, Y] = GlobalTransform(X, Y, posX, posY, phi)
    X = -X;
    
    % from center of back wheels to laser sensor is about 46cm
    Y = Y + 0.46;
    
    deg_heading = phi - 90;
    
    % 2-D Rotation matrix
    A = [ cosd(deg_heading) -sind(deg_heading);
          sind(deg_heading) cosd(deg_heading)];
    
    for j=1:size(X)
       M = A * [X(j); Y(j)];
       X(j) = M(1);
       Y(j) = M(2);
    end
    
    X = X + posX;
    Y = Y + posY;
end

% Function from Project 1 for extracting OOIs

function r = ExtractOOIs(X,Y, intensities)
    r.N         = 0;
    r.Centers   = [];
    r.Colors    = [];
    r.Diameters = [];
    r.ID        = [];
    
    threshold = 0.1;   %if distance between points is greater than 0.1m -> start of new object
    
    %angles = [0:360]'*0.5* pi/180 ;         % associated angle, for each individual range in a scan
    
    i_start = 0;
    i_end = 0;
    for i=1:size(X)
        if (i == 1)
            i_start = i;        % assume first point is start of OOI
        else
           d = sqrt(power(X(i)-X(i-1),2) + power(Y(i)-Y(i-1),2));
           if (d > threshold)
               i_end = i-1;
               % only add to list if there is more than 2 points (otherwise
               % treat this as noise
               if ((i_end - i_start) > 2)
                 %estimate diameter of object as distance between start and
                 %end points
                 diameter = sqrt(power(X(i_end)-X(i_start),2) + power(Y(i_end)-Y(i_start),2));
                 % only store OOI if diameter is between 5 and 20cm
                 if (diameter >= 0.05 && diameter <= 0.2)
                    % check if there are any highly reflective points in this
                    % OOI
                    ii = find(intensities(i_start:i_end)~=0);
                    % if there is, then set the OOI color to 1 (Brilliant)
                    if (size(ii) > 0)
                        r.Colors(1,r.N+1) = 1;
                    else
                        r.Colors(1,r.N+1) = 0;
                    end
                 
                    % Get the centre as the average of x & y values of all the
                    % points
                    avgX = mean(X(i_start:i_end));
                    avgY = mean(Y(i_start:i_end));
                    r.Centers(:,r.N+1) = [avgX, avgY];
                    r.Diameters(1,r.N+1) = diameter;
                    r.N = r.N+1;
                 end
             end
             % start of new OOI
             i_start = i;
           end
        end
    end
return;
end

% Function from Project 1 for plotting OOIs
function PlotOOIs(OOIs,handles)
    if OOIs.N<1, return ; end
    ii = find(OOIs.Colors~=0);
    jj = find(OOIs.Colors==0);
    set(handles.OOIHandle, 'xdata', OOIs.Centers(1,jj), 'ydata', OOIs.Centers(2,jj));
    set(handles.PoleHandle, 'xdata', OOIs.Centers(1,ii), 'ydata', OOIs.Centers(2,ii));
return;
end

% Part D - Data association
% Infers the IDs of all OOIs given in currentOOIs list by comparing their
% distance to all OOIs in the initialOOIs list and matches the closest ones
% together
function R = InferID(currentOOIs, initialOOIs)
    tolerance = 0.4;    % 40cm tolerance
    for i = 1:currentOOIs.N
        min_dist = intmax;
        for j = 1:initialOOIs.N
            dist = sqrt(power((currentOOIs.Centers(1,i) - initialOOIs.Centers(1,j)),2) ...
                      + power((currentOOIs.Centers(2,i) - initialOOIs.Centers(2,j)),2));
            if (dist <= tolerance)
                % infer this initial OOIs ID
                % unless another is closer?
                if (j == 1)
                    min_dist = dist;
                    %infer ID
                    currentOOIs.ID(i) = initialOOIs.ID(j);
                elseif (dist < min_dist)
                    %infer closest ID
                    currentOOIs.ID(i) = initialOOIs.ID(j);
                end
            end
        end
    end
    R = currentOOIs;
end