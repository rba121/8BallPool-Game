Grp11_452Proj
/hw/
/sw/
ip_design_wrapper_bonusincr.xsa          -- Most recent bitstream generated


Inside /hw/ folder, there are 3 sub folders:
adventures_with_ip
ip_repo
LSFR_1.0

Under /adventures_with_ip/ 
		adventures_with_ip.xpr   - which is the Vivado HW project 

Under /ip_repo/
		/bonustimer_1.0/ 	 – Custom Hardware Block 
		/led_controller_1.0/	
		/zed_audio_ctrl/

Inside /sw/ folder, there are 5 sub folders
		452Proj_app_system		 — Vitis Application Project
		452Proj_audio			 — Audio Core (Core 0)
		452Proj_video			—- Video Core (Core 1)
		Rohit_8ball_plat                     —- Most up to date platform project
		Lfsr_video 			—- Test folder for LSFR

Under /lfsr_video/ folder:
		lfsr_test.c 			—- Test file
		
		Under /452Proj_audio/
			src    			—- All source code files

		Under /452Proj_video/
			src    			—- All source code files
		
