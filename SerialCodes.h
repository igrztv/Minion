#ifdef MINION

	#define MOVE 0x85 // 0x85 0x50 0x10 0x60 0xF0 - left wheel speed = 0x50 backward, right wheel speed = 0x60 forward
	#define OBSTACLE 0x70
	#define PATH 0x87 //0x86 0xFF 0xFF
	#define GET_PATH 0x90
	#define GET_POSITION 0x91
	#define SPEED_TEST 0x61
	#define SETTINGS 0x50
	#define SKIP_SETTINGS '0'
	#define PUSH_BRAKES 0x40

	#define MOVE_MANUAL 0x71
	#define MOVE_AUTO 0x72
	
	#define STOPPED 0x20
	
	#define ACC 0x10
	#define Serial_BAUDRATE 115200
	
	#define M0_MAX_SPEED 2951  //[TICKS/S]
	#define M1_MAX_SPEED 3185 //[TICKS/S]
	
	#define M0_MAX_SPEED_DIV_255 11.57 // 2951 / 255 = 11.71
	#define M1_MAX_SPEED_DIV_255 12.49 // 3185 / 255 = 12.71
	#define M0_ACCELERATION 3920
	#define M1_ACCELERATION 3890
	
	#define MMtoTICKS 16.297466 //[TICKS/mm]
	#define TICKStoMM 0.061359 //[mm/TICKS]
	#define WHEELBASE 204 //[mm]
	
	#define TEXT_SKIPPED_SETTINGS "Skipped. To show this settings send "
		
#endif


//
