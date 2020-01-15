#ifndef PSP_TDB_PLOT_REPORT_H
#define PSP_TDB_PLOT_REPORT_H

//#pragma pack (1) //关闭内存对齐

struct MESSAGE_HEADER {

    MESSAGE_HEADER();

    unsigned int        message_code_e;//3301
    unsigned short      message_total_num;//1
    unsigned short      message_index;//1
    unsigned int        message_date;//1
    unsigned int        message_time;//1
    unsigned int        message_length;//结构体长度，字节
};


#ifndef MAX_NUM_ORI_PLOT
#define MAX_NUM_ORI_PLOT 180
#endif

struct NAVIGATION_INFO {

    NAVIGATION_INFO();

    unsigned int	ins_time;			/* 惯导时间，单位: ms */
    int				longitude;			/* 经度，单位: 10^(-6)deg */
    int				latitude;			/* 纬度，单位: 10^(-6)deg */
    unsigned short	ins_data_source;	/* 惯导信息来源，0 */
    short			altitude;			/* 海拔，单位: 0.1m */
    short			yaw_angle;			/* 载机偏航角，单位: 0.01deg */
    short			pitch_angle;		/* 载机俯仰角，单位: 0.01deg */
    short			roll_angle;		    /* 载机横滚角，单位: 0.01deg */
    short			platform_vx;		/* 载机X轴速度分量，单位: 0.01m/s，0 */
    short			platform_vy;		/* 载机Y轴速度分量，单位: 0.01m/s，0 */
    short			platform_vz;		/* 载机Z轴速度分量，单位: 0.01m/s，0 */
};



struct DATA_PROC_PARAM {

    DATA_PROC_PARAM();

    unsigned int		serial_number;		/* 波束序列号 */
    unsigned int		base_time;			/* 对时时间，单位: ms */
    unsigned short int	beam_pos;			/* 波位号 ，从零度开始，顺时针编号 */
    unsigned short int	work_mode;			/* 工作模式，见WORK_MODE_E */
    unsigned short int	frame_num;			/* 按空海类型分开后的帧编号 */
    unsigned short int	frame_num_whole;	/* 总帧编号，扫描圈数 */
    unsigned short int	frame_start;		/* 帧开始标志 */
    unsigned short int	frame_end;			/* 帧结束标志 */
    unsigned short int	track_id;			/* 航迹编号，跟踪波束使用 */
    unsigned short int	track_plot_id;		/* 航迹对应的点迹的编号，TRK离线调试用 */
    short int			horizon_center;	    /* 波束的水平中心指向，阵面坐标系，单位: 0.01deg， 与波位号对应：1：0，2：1000，3：2000，4：3000，5：4100，6：5100，7：6000，8：35000，9：34000，10：33000，11：31900，12：30900，13：30000*/
    short int			vertical_center;	/* 波束的俯仰中心指向，阵面坐标系，单位: 0.01deg，30*0.01 = 3000 */
    unsigned short int	band_width;		    /* 带宽，单位: 0.1MHz，10 */
    unsigned short int	work_freq;			/* 工作频点，2400 */
    unsigned short int	simu_flag;			/* 模拟目标标志，0 */
    unsigned short int	blind_distance;	    /* 近距离盲区，单位: km，0 */
    unsigned short int	range_distance;	    /* 距离量程，单位: km，100 */
    unsigned int 		rng_unitstart;      /* 0 */
       unsigned int		rng_unitend;        /* 0 */
    unsigned char		spare[6];		    /* 预留 */
};



struct ORI_BURST_HEADER {

    ORI_BURST_HEADER();

    unsigned  int		num_plot;			/* 点迹个数 */
    unsigned int	    sob_time;	        /* 波束发出时间，硬件时间，取计算机时间 */
    unsigned  int		servo_point;		/* 伺服指向，0-8192， 0*/
    unsigned  int		noise_ad_uh;		/* 背景噪声，单位: 0.1dB，需要*/ //50
    unsigned  int		cfar_thrd;			/* CFAR门限，单位: 0.1dB */ //130
    unsigned int		psp_error_code;	    /* PSP错误代码 */
    unsigned int        check_id;			/*检测号*/
    unsigned int        merge_flag;			/*凝聚标志 凝聚1 不凝聚 0*/
    unsigned int        video_flag;			/*一次视频标志，1：有一次视频数据 0没有*/
    unsigned int		pspxc_flag;			/* 相缠    1 0  非		 预留 */
    int                 rng_diff;
    unsigned int		ant_perform;		/* 1 不捷变 5捷变，1*/
};


struct ORI_PLOT_DATA {

    ORI_PLOT_DATA();

    unsigned int		id;			    /* 原始点id               0    */
    unsigned int		range;		    /*  凝聚后的点此处填入凝聚后中心距离值 ,探测距离 单位 0.01米 */
    short			    hor_angle_meas;	/* 测角值，单位:0.001度         凝聚后的点此处填入凝聚后中心测角值*/
    short			    radial_freq;	/* 径向频率，单位: Hz */
    unsigned  int	    ad_atten;		/* AD幅度，单位: 0.1dB，检测强度   凝聚后的点此处填入凝聚后幅度值*/ //160
    unsigned  int	    noise_ad;		/* 背景噪声，单位: 0.1dB */ //50
    unsigned  int	    cfar_thrd;		/* CFAR门限，单位: 0.1dB */ //130
    unsigned int	    merageNum;		/*凝聚的点的数量*/
    unsigned int		prsinRngFlg;    /*点迹精确距离标志，1表示精确，0表示不精确*/
    unsigned int		min_range;      /*凝聚点的最远径向距离 单位 0.01米*/
    short  max_delta_az;                //最大凝聚方位 0.001度 测角值的凝聚
    short  min_delta_az;                //最小凝聚方位 0.001度 测角值的凝聚
    short sparebak;
    unsigned short       plot_servo_point;//伺服码盘差值 ，幅度最大的脉冲减波束中心伺服值。单位360/8192
    unsigned int		plot_polarize;	  /* 1 vv 2 hh 3 vh 4 hv，1 */
};


struct PSP_TBD_PLOT_REPORT {
    PSP_TBD_PLOT_REPORT();
    MESSAGE_HEADER		message_header;//
    NAVIGATION_INFO		navigation_info;	/* 当前最新惯导信息 */
    DATA_PROC_PARAM     data_proc_param;	/* 数据处理参数 */
    ORI_BURST_HEADER	ori_burst_header;
    ORI_PLOT_DATA		ori_plot_data[MAX_NUM_ORI_PLOT];
};



#endif // PSP_TDB_PLOT_REPORT_H
