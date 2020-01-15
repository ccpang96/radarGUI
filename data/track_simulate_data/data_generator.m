%% UKF bicycle test
clear all
close all

%% 初始化

% 设置初始位置

%p_x_start = 0.6;
%p_y_start = 0.6;

p_x_start = 10;
p_y_start = 10;

base_timestamp = 0; % 初试时间 单位是us 
delta_t_us = 0.5*1e5; % 时间刻度t 单位是us
delta_t_sec = delta_t_us / 1e6; %时间刻度t 单位是s
round_time_sec = 25; %周期

n_z = round_time_sec/delta_t_sec;  % 测量数据总点数
n_x = 5;  % 状态维数

%真实值
GT = zeros(n_x,n_z); % 真实值: p1 p2 v_abs yaw yaw_dot v1 v2  x方向位置 y方向位置 切向速度v 角度yaw 角度变化率yaw_dot x方向速度 y方向速度
%激光和毫米波雷达的测量值
Z_l = zeros(3,n_z); % 激光雷达测量值: pos1, pos2 time                 
Z_r = zeros(4,n_z); % 毫米波雷达测量值: r, phi, r_dot time

% 设置起始值
GT(1,1) =  p_x_start;
GT(2,1) =  p_y_start;

%测量误差
std_las1 = 0.15;
std_las2 = 0.15;

std_radr = 0.3;
std_radphi = 0.03;
std_radrd = 0.3;

for k = 1:n_z
    timestamp = base_timestamp+(k-1)*delta_t_us; %经过0.05s采集一次数据
    round_time_sec = 25; % 周期
    % 时间
    GT(8,k) = timestamp;
    % 改变速度
    time_sec = k*delta_t_us/1e6;
 
  
   %第0批数据
    %GT(3,k) = 2 + 1.6*sin(2*2*pi/round_time_sec*time_sec); %汽车速度
    % 角度变化率
    %GT(5,k) = 1.55*sin(2*pi/round_time_sec*time_sec);  %汽车角度变换率
    
    
    %第一批数据：
    %GT(3,k) = 100; %速度是100
    %GT(5,k) = 0.05 * (2*pi/round_time_sec*time_sec); %仿真radarGUI又添加的

    GT(3,k) = 100 + 1.6*sin(2*2*pi/round_time_sec*time_sec); %汽车速度
    % 角度变化率
    GT(5,k) = 0.65*sin(2*pi/round_time_sec*time_sec);  %汽车角度变换率
    
    % yaw rate(角加速度)
    %GT(5,k) = 0.55*sin(2*pi/round_time_sec*time_sec);
end

Z_l(3,1)=base_timestamp;
Z_r(4,1)=base_timestamp;

%% 第一个测量值 (特殊情况) 
% 激光雷达
Z_l(1,1) = GT(1,1) + normrnd(0, std_las1) ;
Z_l(2,1) = GT(2,1) + normrnd(0, std_las2) ;
Z_l(3,1) = GT(8,1) ;  % 时间

% 毫米波雷达测量值
p1 = GT(1,1);
p2 = GT(2,1);
v =  GT(3,1);
yaw = GT(4,1);

v1 = GT(3,1) * cos(GT(4,1));
v2 = GT(3,1) * sin(GT(4,1));

Z_r(1,1) = sqrt(p1^2 + p2^2) + normrnd(0, std_radr) ;                        %r
Z_r(2,1) = atan2(p2,p1) + normrnd(0, std_radphi) ;                           %phi
Z_r(3,1) = (p1*v1 + p2*v2 ) / sqrt(p1^2 + p2^2) + normrnd(0, std_radrd) ;    %r_dot
Z_r(4,1) = GT(8,1) ;  % time

GT(6,1) = cos(yaw)* v; % v_x
GT(7,1) = sin(yaw)* v; % v_y
%%
% 定位和测量
for k = 2:n_z
    
    delta_t_s = (GT(8,k) - GT(8,k-1))/1e6;
    
    p1 = GT(1,k-1);
    p2 = GT(2,k-1);
    v = GT(3,k-1);
    yaw = GT(4,k-1);
    yaw_dot = GT(5,k-1);
    
    if abs(yaw_dot) > 0.001   
        p1_p = p1 + v/yaw_dot * ( sin (yaw + yaw_dot*delta_t_s) - sin(yaw)); 
        p2_p = p2 + v/yaw_dot * ( cos(yaw) - cos(yaw+yaw_dot*delta_t_s) );
    else
        p1_p = p1 + v*delta_t_s*cos(yaw);
        p2_p = p2 + v*delta_t_s*sin(yaw);
    end
    v_p = v;
    yaw_p = yaw + yaw_dot*delta_t_s;
    yaw_dot_p = yaw_dot;
    
    GT(1,k) = p1_p;
    GT(2,k) = p2_p;
    GT(4,k) = yaw_p;

    % 激光雷达测量值    
    Z_l(1,k) = GT(1,k) + normrnd(0, std_las1) ;
    Z_l(2,k) = GT(2,k) + normrnd(0, std_las2) ;
    Z_l(3,k) = GT(8,k) ;  % time

    % 毫米波雷达测量值
    p1 = GT(1,k);  %vx
    p2 = GT(2,k);  %vy
    v = GT(3,k);   %v
    yaw = GT(4,k); %角度

    v1 = GT(3,k) * cos(GT(4,k));
    v2 = GT(3,k) * sin(GT(4,k));

    Z_r(1,k) = sqrt(p1^2 + p2^2) + normrnd(0, std_radr) ;  %r
    Z_r(2,k) = atan2(p2,p1) + normrnd(0, std_radphi) ;   %角度，如果是在第一象限，就是0~pi/2，如果是第二象限，就是pi/2 ~ pi; 如果是第三象限，就是-pi~-pi/2; 如果是第四象限，就是-pi/2 ~0; 
    %对数据进行一次转换
    %{
    if Z_r(2,k) > 0 %位于第一和第二象限 
        Z_r(2,k) = 180 * Z_r(2,k) / pi;
    else 
        Z_r(2,k) =  360 - 180 * Z_r(2,k) / pi;
    end 
    %}
    Z_r(3,k) = (p1*v1 + p2*v2 ) / sqrt(p1^2 + p2^2) + normrnd(0, std_radrd) ;  %r_dot
    Z_r(4,k) = GT(8,k) ;  % time
    
    GT(6,k) = cos(yaw)* v; % v_x
    GT(7,k) = sin(yaw)* v; % v_y
end

%% 生成真实值文件
GTT=GT';

%真实值 - p1 p2 v_abs yaw yaw_dot v1 v2
fileID = fopen('Z:\sister\radarGUI\generator_data\obj_pose-laser-radar-synthetic-gt.txt','w');
[nrows,ncols] = size(GTT);
for row = 1:nrows
    fprintf(fileID,'%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n',GTT(row,1:8));
end
fclose(fileID);

%% 生成测量输入文件 
%add timestamps
%L    p1    p2    timestamp=
%R    r theta    r_dot    timestamp

%laser structure
Z_lT = Z_l';
N_L =size(Z_lT,1);
v_z = zeros(N_L,1); %vector of zeros
Z_lT = [Z_lT, v_z];
%create a laser cell array - mixed numbers and characters
C_l = cell(N_L,1);
C_l(:) = {'L'};
C_l = [ C_l, num2cell(Z_lT)];

%radar structure
Z_rT = Z_r';
N_R =size(Z_rT,1);
%create a radar cell array - mixed numbers and characters
C_r = cell(N_R,1);
C_r(:) = {'R'};
C_r = [ C_r, num2cell(Z_rT)];


AB ={};
for i = 1:size(Z_lT,1)
    %从各自矩阵中不断添加值进去
    AB = [AB; C_l(i,:)];
    AB = [AB; C_r(i,:)];
end


%% 输出数据格式是融合的方式：一个激光雷达数据、一个毫米波雷达数据
fileID = fopen('Z:\sister\radarGUI\generator_data\laser_radar.txt','w');
[nrows,ncols] = size(AB);
row_gt = int16(1);

for row = 1:nrows
    if( AB{row,1} == 'L' &&  mod(row,4) == 1 )
        
        %测量值
        %fprintf('%s\t%.6f\t%.6f\t%d\n',AB{row,1:ncols-1});
        fprintf(fileID,'%s\t%.6f\t%.6f\t%d\t',AB{row,1:ncols-1});
        %真实值位置和速度
        %fprintf('%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy 
        fprintf(fileID,'%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy 
        
        %角度yaw和角度变化率yaw_rate 
        % fprintf(fileID,'%d\t%d\n',GTT(row_gt, [4 5]));
        
    elseif( AB{row,1} == 'R' &&  mod(row,4) == 0 )
        
        %模拟测量值
       % fprintf('%s\t%.6f\t%.6f\t%.6f\t%d\n',AB{row,:});
        fprintf(fileID,'%s\t%.6f\t%.6f\t%.6f\t%d\t',AB{row,:});
        %真实位置和速度值
       % fprintf('%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy ;
        fprintf(fileID,'%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy ;
         
    end
    if(mod(row,2) == 0) %一个真实值分别对应雷达数据和激光雷达数据
        row_gt = row_gt + 1;
    end;
end

fclose(fileID); 


%% 输出数据格式是只输出激光雷达数据
fileID = fopen('Z:\sister\radarGUI\generator_data\input_lidar.txt','w');
[nrows,ncols] = size(AB);
row_gt = int16(1);

for row = 1:nrows
    if( AB{row,1} == 'L' )
        
        %测量值
      
        fprintf(fileID,'%s\t%.6f\t%.6f\t%d\t',AB{row,1:ncols-1});
        %真实值位置和速度
        fprintf(fileID,'%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy 
    end
    if(mod(row,2) == 0) %一个真实值分别对应雷达数据和激光雷达数据
        row_gt = row_gt + 1;
    end;
end

fclose(fileID); 


%% 输出数据格式是只输出毫米波雷达数据
fileID = fopen('Z:\sister\radarGUI\data\input_radar.txt','w');
[nrows,ncols] = size(AB);
row_gt = int16(1);

for row = 1:nrows
    if( AB{row,1} == 'R' )
        
        %模拟测量值
       % fprintf('%s\t%.6f\t%.6f\t%.6f\t%d\n',AB{row,:});
        %fprintf(fileID,'%s\t%.6f\t%.6f\t%.6f\t%d\t',AB{row,:});
        fprintf(fileID,'%d\t',AB{row,5});
        fprintf(fileID,'%.6f\t%.6f\t%.6f\t',AB{row,2:4});
        %真实位置和速度值
       % fprintf('%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy ;
       % fprintf(fileID,'%.6f\t%.6f\t%.6f\t%.6f\n',GTT(row_gt, [1 2 6 7]));  %真实px py vx vy ;
       fprintf(fileID,'%.6f\t%.6f\n',GTT(row_gt,[1,2]));
    end
    if(mod(row,2) == 0) %一个真实值分别对应雷达数据和激光雷达数据
        row_gt = row_gt + 1;
    end;
end

fclose(fileID); 




% 将工作空间保存下来
save('Z:\sister\radarGUI\generator_data\bicycle_data.mat') 

figure(2)
hold on;
plot(GT(1,:), GT(2,:), '-og'); 

xlabel('x');
ylabel('y');
axis equal
legend('GT')

%%
figure(1)
hold on;
plot(GT(1,:), '.-k');  %py
plot(GT(2,:), '.-b');  %py
plot(GT(3,:), '.-g');  %v
plot(GT(4,:), '.-r');  %yaw
plot(GT(5,:), '.-m');  %yawrate 
plot(diff(GT(3,:))/delta_t_sec, '-c'); %acc
plot(diff(GT(5,:))/delta_t_sec, '.c'); %yawacc
legend('px','py', 'v', 'yaw', 'yawrate', 'acc', 'yawacc')

