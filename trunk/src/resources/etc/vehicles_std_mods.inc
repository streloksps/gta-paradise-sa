#���������� � ����� ��� ���� � ���

# ��������� ����������� ����������
cmp_wheels=wheel_gn1,wheel_gn2,wheel_gn3,wheel_gn4,wheel_gn5,wheel_lr1,wheel_lr2,wheel_lr3,wheel_lr4,wheel_lr5,wheel_or1,wheel_sr1,wheel_sr2,wheel_sr3,wheel_sr4,wheel_sr5,wheel_sr6
cmp_ntos=nto_b_s,nto_b_l,nto_b_tw
cmp_exhaust=exh_b_s,exh_b_m,exh_b_l,exh_b_t,exh_b_ts
cmp_std_tune=$(cmp_wheels),$(cmp_ntos),hydralics

# ������ ����������� ����������� (�� ����� carmods.dat)
std_mods_landstal=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,exh_b_ts,lgt_b_rspt,lgt_b_sspt
std_mods_bravura=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_t,lgt_b_rspt,rf_b_sc_r,spl_b_bab_m,spl_b_mab_m,wg_l_b_ssk
std_mods_buffalo=$(cmp_std_tune)
std_mods_peren=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,lgt_b_rspt,spl_b_bar_m,spl_b_bbr_m,spl_b_mar_m,wg_l_b_ssk
std_mods_sentinel=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,exh_b_ts,spl_b_bab_m,spl_b_bar_l,spl_b_bbb_m,spl_b_mar_m
std_mods_stretch=$(cmp_std_tune)
std_mods_manana=$(cmp_std_tune),exh_b_t,exh_b_m,exh_b_l,lgt_b_rspt,lgt_b_sspt,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_infernus=$(cmp_std_tune)
std_mods_voodoo=$(cmp_std_tune)
std_mods_cheetah=$(cmp_std_tune),exh_b_t,exh_b_ts,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_moonbeam=$(cmp_std_tune),exh_b_l,exh_b_m,rf_b_sc_r,spl_b_bar_m,spl_b_bbr_m
std_mods_esperant=$(cmp_std_tune)
std_mods_taxi=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,exh_b_m,exh_b_t,spl_b_bab_m,spl_b_mab_m
std_mods_washing=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,exh_b_ts,spl_b_bar_l,spl_b_bbb_m,spl_b_bbr_m,spl_b_mar_m
std_mods_bobcat=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,lgt_b_rspt,wg_l_b_ssk
std_mods_premier=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,exh_b_m,exh_b_t,rf_b_sc_r,spl_b_bab_m,spl_b_mab_m
std_mods_banshee=$(cmp_std_tune)
std_mods_previon=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_s,exh_b_t,lgt_b_rspt,rf_b_sc_r,spl_b_bab_m,spl_b_mab_m,wg_l_b_ssk
std_mods_cabbie=$(cmp_std_tune)
std_mods_stallion=$(cmp_std_tune),spl_b_mab_m,spl_b_bbb_m,spl_b_bab_m,wg_l_b_ssk,bntl_b_ov,bntl_b_sq,lgt_b_rspt
std_mods_romero=$(cmp_std_tune)
std_mods_admiral=$(cmp_std_tune)
std_mods_turismo=$(cmp_std_tune)
std_mods_solair=$(cmp_std_tune)
std_mods_glendale=$(cmp_std_tune)
std_mods_oceanic=$(cmp_std_tune)
std_mods_hermes=$(cmp_std_tune)
std_mods_sabre=$(cmp_std_tune)
std_mods_zr350=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,exh_b_ts,rf_b_sc_r,wg_l_b_ssk
std_mods_walton=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,bnt_b_sc_p_l,exh_b_l,exh_b_m,exh_b_s,lgt_b_rspt,lgt_b_sspt
std_mods_regina=$(cmp_std_tune)
std_mods_comet=$(cmp_std_tune)
std_mods_rancher=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,exh_b_l,exh_b_t,exh_b_ts,lgt_b_rspt,lgt_b_sspt,rf_b_sc_r,spl_b_bar_m,spl_b_bbr_m,spl_b_mar_m
std_mods_virgo=$(cmp_std_tune),bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_m,exh_b_t,exh_b_ts,spl_b_bar_l,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_greenwoo=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,rf_b_sc_r,spl_b_bbr_m,spl_b_mar_m
std_mods_blistac=$(cmp_std_tune),rf_b_sc_r,wg_l_b_ssk,bnt_b_sc_p_m,exh_b_t,spl_b_bbb_m,spl_b_bab_m,spl_b_mab_m,spl_b_bar_m,bntl_b_ov,exh_b_l
std_mods_mesa=$(cmp_std_tune),exh_b_l,exh_b_m,exh_b_t,lgt_b_rspt,lgt_b_sspt
std_mods_supergt=$(cmp_std_tune)
std_mods_elegant=$(cmp_std_tune)
std_mods_nebula=$(cmp_std_tune),bnt_b_sc_m,exh_b_l,exh_b_m,exh_b_t,exh_b_ts,spl_b_bar_m,spl_b_bbr_l,spl_b_bbr_m,spl_b_mar_m,wg_l_b_ssk
std_mods_majestic=$(cmp_std_tune),bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_t,exh_b_ts,spl_b_bar_m,spl_b_bbb_m,spl_b_bbr_m,spl_b_mab_m,wg_l_b_ssk
std_mods_buccanee=$(cmp_std_tune),bnt_b_sc_l,bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_ts,lgt_b_rspt,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_fortune=$(cmp_std_tune)
std_mods_cadrona=$(cmp_std_tune),exh_b_m,exh_b_l,exh_b_ts,spl_b_bab_m,spl_b_bar_l,spl_b_bbr_l,wg_l_b_ssk
std_mods_willard=$(cmp_std_tune),bnt_b_sc_p_l,bnt_b_sc_p_m,exh_b_l,exh_b_t,exh_b_ts,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_feltzer=$(cmp_std_tune)
std_mods_remingtn=$(cmp_std_tune),exh_lr_rem1,exh_lr_rem2,fbmp_lr_rem1,fbmp_lr_rem2,misc_c_lr_rem1,misc_c_lr_rem2,misc_c_lr_rem3,rbmp_lr_rem1,rbmp_lr_rem2,wg_l_lr_rem1,wg_l_lr_rem2
std_mods_slamvan=$(cmp_std_tune),bbb_lr_slv1,bbb_lr_slv2,exh_lr_slv1,exh_lr_slv2,fbb_lr_slv1,fbb_lr_slv2,fbmp_lr_slv1,wg_l_lr_slv1,wg_l_lr_slv2
std_mods_blade=$(cmp_std_tune),exh_lr_bl1,exh_lr_bl2,fbmp_lr_bl1,fbmp_lr_bl2,rbmp_lr_bl1,rbmp_lr_bl2,rf_lr_bl1,rf_lr_bl2,wg_l_lr_bl1
std_mods_vincent=$(cmp_std_tune),bnt_b_sc_m,bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_t,exh_b_ts,lgt_b_sspt,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,wg_l_b_ssk
std_mods_bullet=$(cmp_std_tune)
std_mods_clover=$(cmp_std_tune),bntl_b_sq,exh_b_l,exh_b_m,exh_b_t,exh_b_ts,spl_b_bar_l,spl_b_bbr_l
std_mods_hustler=$(cmp_std_tune)
std_mods_intruder=$(cmp_std_tune),bnt_b_sc_m,bntl_b_ov,bntl_b_sq,exh_b_t,exh_b_ts,lgt_b_sspt,rf_b_sc_r,spl_b_bar_m,spl_b_bab_m,spl_b_bbb_m,wg_l_b_ssk
std_mods_primo=$(cmp_std_tune),bntl_b_ov,exh_b_l,exh_b_m,exh_b_t,exh_b_ts,spl_b_bbr_m,spl_b_mab_m,spl_b_mar_m
std_mods_tampa=$(cmp_std_tune),bnt_b_sc_p_l,bnt_b_sc_p_m,bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_t,exh_b_ts,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_sunrise=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_t,exh_b_ts,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m
std_mods_merit=$(cmp_std_tune),bnt_b_sc_l,exh_b_l,exh_b_m,exh_b_t,exh_b_ts,rf_b_sc_r,spl_b_bar_m,spl_b_bbb_m,spl_b_bbr_m,spl_b_mab_m
std_mods_windsor=$(cmp_std_tune)
std_mods_uranus=$(cmp_std_tune),exh_a_u,exh_c_u,fbmp_a_u,fbmp_c_u,rbmp_a_u,rbmp_c_u,rf_a_u,rf_c_u,spl_a_u_b,spl_c_u_b,wg_l_a_u,wg_l_c_u
std_mods_jester=$(cmp_std_tune),exh_a_j,exh_c_j,fbmp_a_j,fbmp_c_j,rbmp_a_j,rbmp_c_j,spl_a_j_b,spl_c_j_b,rf_a_j,rf_c_j,wg_l_a_j,wg_l_c_j
std_mods_sultan=$(cmp_std_tune),exh_a_s,exh_c_s,fbmp_a_s,fbmp_c_s,rbmp_a_s,rbmp_c_s,rf_a_s,rf_c_s,spl_a_s_b,spl_c_s_b,wg_l_a_s,wg_l_c_s
std_mods_stratum=$(cmp_std_tune),exh_a_st,exh_c_st,fbmp_a_st,fbmp_c_st,rbmp_a_st,rbmp_c_st,rf_a_st,rf_c_st,spl_a_st_r,spl_c_st_r,wg_l_a_st,wg_l_c_st
std_mods_elegy=$(cmp_std_tune),exh_a_l,exh_c_l,fbmp_a_l,fbmp_c_l,rbmp_a_l,rbmp_c_l,rf_a_l,rf_c_l,spl_a_l_b,spl_c_l_b,wg_l_a_l,wg_l_c_l
std_mods_flash=$(cmp_std_tune),exh_a_f,exh_c_f,fbmp_a_f,fbmp_c_f,rbmp_a_f,rbmp_c_f,rf_a_f,rf_c_f,spl_a_f_r,spl_c_f_r,wg_l_a_f,wg_l_c_f
std_mods_tahoma=$(cmp_std_tune)
std_mods_savanna=$(cmp_std_tune),exh_lr_sv1,exh_lr_sv2,fbmp_lr_sv1,fbmp_lr_sv2,rbmp_lr_sv1,rbmp_lr_sv2,rf_lr_sv1,rf_lr_sv2,wg_l_lr_sv
std_mods_broadway=$(cmp_std_tune),exh_lr_br1,exh_lr_br2,fbmp_lr_br1,fbmp_lr_br2,rbmp_lr_br1,rbmp_lr_br2,wg_l_lr_br1
std_mods_tornado=$(cmp_std_tune),exh_lr_t1,exh_lr_t2,fbmp_lr_t1,fbmp_lr_t2,rbmp_lr_t1,rbmp_lr_t2,wg_l_lr_t1
std_mods_huntley=$(cmp_std_tune)
std_mods_stafford=$(cmp_std_tune),exh_b_l,exh_b_ts,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,wg_l_b_ssk
std_mods_emperor=$(cmp_std_tune),bntl_b_ov,bntl_b_sq,exh_b_l,exh_b_t,exh_b_ts,lgt_b_rspt,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,spl_b_mab_m,wg_l_b_ssk
std_mods_euros=$(cmp_std_tune)
std_mods_club=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,bntl_b_sq,exh_b_l,exh_b_ts,lgt_b_sspt,lgt_b_rspt,rf_b_sc_r,spl_b_bbr_m,spl_b_mar_m,wg_l_b_ssk
std_mods_picador=$(cmp_std_tune),bnt_b_sc_l,bnt_b_sc_m,exh_b_l,exh_b_s,exh_b_ts,lgt_b_rspt,rf_b_sc_r,wg_l_b_ssk
std_mods_alpha=$(cmp_std_tune)
std_mods_phoenix=$(cmp_std_tune),bntl_b_sq,bntl_b_ov,exh_b_l,exh_b_t,exh_b_ts,lgt_b_sspt,rf_b_sc_r,spl_b_bab_m,spl_b_bbb_m,wg_l_b_ssk

# ������ ����������� ������ (���� carcols.dat)
std_cols_landstal=4,1, 123,1, 113,1, 101,1, 75,1, 62,1, 40,1, 36,1
std_cols_bravura=41,41, 47,47, 52,52, 66,66, 74,74, 87,87,91,91, 113,113
std_cols_buffalo=10,10, 13,13, 22,22, 30,30, 39,39, 90,90, 98,98, 110,110
std_cols_linerun=36,1, 37,1, 30,1, 28,1, 25,1, 40,1, 101,1, 113,1
std_cols_peren=113,39, 119,50, 123,92, 109,100, 101,101, 95,105, 83,110, 66,25
std_cols_sentinel=11,1, 24,1, 36,1, 40,1, 75,1, 91,1, 123,1, 4,1
std_cols_dumper=1,1
std_cols_firetruk=3,1
std_cols_trash=26,26
std_cols_stretch=1,1
std_cols_manana=4,1, 9,1, 10,1, 25,1, 36,1, 40,1, 45,1, 84,1
std_cols_infernus=12,1, 64,1, 123,1, 116,1, 112,1, 106,1, 80,1, 75,1
std_cols_voodoo=9,1, 10,8, 11,1, 25,8, 27,1, 29,8, 30,1, 37,8
std_cols_pony=87,1, 88,1, 91,1, 105,1, 109,1, 119,1, 4,1, 25,1
std_cols_mule=25,1, 28,1, 43,1, 67,1, 72,1, 9,1, 95,1, 24,1
std_cols_cheetah=20,1, 25,1, 36,1, 40,1 62,1, 75,1, 92,1, 0,1
std_cols_ambulan=1,3
std_cols_moonbeam=119,119, 117,227, 114,114, 108,108, 95,95, 81,81, 61,61, 41,41
std_cols_esperant=45,75, 47,76, 33,75, 13,76, 54,75, 69,76, 59,75, 87,76
std_cols_taxi=6,1
std_cols_washing=4,1, 13,1, 25,1, 30,1, 36,1, 40,1, 75,1, 95,1
std_cols_bobcat=96,25, 97,25, 101,25, 111,31, 113,36, 83,57, 67,59
std_cols_mrwhoop=1,16, 1,56, 1,17, 1,53, 1,5, 1,35
std_cols_bfinject=1,0, 2,2, 3,2, 3,6, 6,16, 15,30, 24,53, 35,61
std_cols_hunter=43,0
std_cols_premier=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_enforcer=7,1
std_cols_securica=4,75
std_cols_banshee=12,12, 13,13, 14,14, 1,2, 2,1, 1,3, 3,1, 10,10
std_cols_predator=46,26
std_cols_bus=71,59, 75,59, 92,72, 47,74, 55,83, 59,83, 71,87, 82,87
std_cols_rhino=43,0
std_cols_barracks=43,0
std_cols_hotknife=1,1, 12,12, 2,2, 6,6, 4,4, 46,46, 53,53
std_cols_artict1=1,1
std_cols_previon=83,1, 87,1, 92,1, 95,1, 109,1, 119,45, 11,1
std_cols_coach=54,7, 79,7, 87,7, 95,16, 98,20, 105,20, 123,20, 125,21
std_cols_cabbie=6,76
std_cols_stallion=57,8, 8,17, 43,21, 54,38, 67,8, 37,78, 65,79, 25,78
std_cols_rumpo=34,34, 32,32, 20,20, 110,110, 66,66, 84,84, 118,118, 121,121
std_cols_rcbandit=2,96, 79,42, 82,54, 67,86, 126,96, 70,96, 110,54, 67,98
std_cols_romero=0,0, 11,105, 25,109, 36,0, 40,36, 75,36, 0,36, 0,109
std_cols_packer=4,1, 20,1, 24,1, 25,1, 36,1, 40,1, 54,1, 84,1
std_cols_monster=32,36, 32,42, 32,53, 32,66, 32,14, 32,32
std_cols_admiral=34,34, 35,35, 37,37, 39,39, 41,41, 43,43, 45,45, 47,47
std_cols_squalo=0,0, 1,5, 3,3, 1,22, 1,35, 1,44, 1,53, 1,57
std_cols_seaspar=75,2
std_cols_pizzaboy=3,6
std_cols_tram=1,74
std_cols_artict2=1,1
std_cols_turismo=123,123, 125,125, 36,36, 16,16, 18,18, 46,46, 61,61, 75,75
std_cols_speeder=1,3, 1,5, 1,16, 1,22, 1,35, 1,44, 1,53, 1,57
std_cols_reefer=56,56
std_cols_tropic=26,26
std_cols_flatbed=84,15, 84,58, 84,31, 32,74, 43,31, 1,31, 77,31, 32,74
std_cols_yankee=84,63, 91,63, 102,65, 105,72, 110,93, 121,93, 12,95, 23,1
std_cols_caddy=58,1, 2,1, 63,1, 18,1, 32,1, 45,1, 13,1, 34,1
std_cols_solair=91,1, 101,1, 109,1, 113,1, 4,1, 25,1, 30,1, 36,1
std_cols_topfun=26,26, 28,28, 44,44, 51,51, 57,57, 72,72, 106,106, 112,112
std_cols_skimmer=1,3, 1,9, 1,18, 1,30, 17,23, 46,23, 46,32, 57,34
std_cols_pcj600=36,1, 37,1, 43,1, 53,1, 61,1, 75,1, 79,1, 88,1
std_cols_faggio=12,12, 13,13, 14,14, 1,2, 2,1, 1,3, 3,1, 10,10
std_cols_freeway=79,79, 84,84, 7,7, 11,11, 19,19, 22,22, 36,36, 53,53
std_cols_rcbaron=14,75
std_cols_rcraider=14,75
std_cols_glendale=67,76, 68,76, 78,76, 2,76, 16,76, 18,76, 25,76, 45,88
std_cols_oceanic=51,1, 58,8, 60,1, 68,8, 2,1, 13,8, 22,1, 36,8
std_cols_sanchez=6,6, 46,46, 53,53, 3,3
std_cols_sparrow=1,3
std_cols_patriot=43,0
std_cols_quad=120,117, 103,111, 120,114, 74,91, 120,112, 74,83, 120,113, 66,71
std_cols_coastg=56,15, 56,53
std_cols_dinghy=56,15, 56,53
std_cols_hermes=97,1, 81,1, 105,1, 110,1, 91,1, 74,1, 84,1, 83,1
std_cols_sabre=2,39, 9,39, 17,1, 21,1, 33,0, 37,0, 41,29, 56,29
std_cols_rustler=6,7, 7,6, 1,6, 89,91, 119,117, 103,102, 77,87, 71,77
std_cols_zr350=92,1, 94,1, 101,1, 121,1, 0,1, 22,1, 36,1, 75,1
std_cols_walton=72,1, 66,1, 59,1, 45,1, 40,1, 39,1, 35,1, 20,1
std_cols_regina=27,36, 59,36, 60,35, 55,41, 54,31, 49,23, 45,32, 40,29
std_cols_comet=73,45, 12,12, 2,2, 6,6, 4,4, 46,46, 53,53
std_cols_bmx=1,1, 3,3, 6,6, 46,46, 65,9, 14,1, 12,9, 26,1
std_cols_burrito=41,41, 48,48, 52,52, 64,64, 71,71, 85,85, 10,10, 62,62
std_cols_camper=1,31, 1,20, 1,5, 0,6, 3,6, 16,0, 17,0
std_cols_marquis=12,35, 50,32, 40,26, 66,36
std_cols_baggage=1,73, 1,74, 1,75, 1,76, 1,77, 1,78, 1,79
std_cols_dozer=1,1
std_cols_maverick=26,14, 29,42, 26,57, 54,29, 26,3, 3,29, 12,39, 74,35
std_cols_vcnmav=2,26, 2,29
std_cols_rancher=13,118, 14,123, 120,123, 112,120, 84,110, 76,102
std_cols_fbiranch=0,0
std_cols_virgo=40,65, 71,72, 52,66, 64,72, 30,72, 60,72
std_cols_greenwoo=30,26, 77,26, 81,27, 24,55, 28,56, 49,59, 52,69, 71,107
std_cols_jetmax=36,13
std_cols_hotring=36,117, 36,13, 42,30, 42,33, 54,36, 75,79, 92,101, 98,109
std_cols_sandking=123,124, 119,122, 118,117, 116,115, 114,108, 101,106, 88,99, 5,6
std_cols_blistac=74,72, 66,72, 53,56, 37,19, 22,22, 20,20, 9,14, 0,0
std_cols_polmav=0,1
std_cols_boxville=11,123, 13,120, 20,117, 24,112, 27,107, 36,105, 37,107, 43,93
std_cols_benson=109,25, 109,32, 112,32, 10,32, 30,44, 32,52, 84,66, 84,69
std_cols_mesa=75,84, 40,84, 40,110, 28,119, 25,119, 21,119, 13,119, 4,119
std_cols_rcgoblin=14,75
std_cols_hotrina=7,94, 36,88, 51,75, 53,75 ,58,67, 75,67, 75,61, 79,62
std_cols_hotrinb=83,66, 87,74, 87,75, 98,83, 101,100, 103,101, 117,116, 123,36
std_cols_bloodra=51,39, 57,38, 45,29, 34,9, 65,9, 14,1, 12,9, 26,1
std_cols_rnchlure=13,118, 14,123, 120,123, 112,120, 84,110, 76,102
std_cols_supergt=3,3, 6,6, 7,7, 52,52, 76,76
std_cols_elegant=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_journey=1,1
std_cols_bike=7,1, 74,1, 61,1, 16,1, 25,1, 30,1, 36,1, 53,1
std_cols_mtbike=43,43, 46,46, 39,39, 28,28, 16,16, 6,6, 5,5, 2,2
std_cols_beagle=3,90, 4,90, 7,68, 8,66, 12,60, 27,97, 34,51, 37,51
std_cols_cropdust=17,39, 15,123, 32,112, 45,88, 52,71, 57,67, 61,96, 96,96
std_cols_stunt=38,51, 21,36, 21,34, 30,34, 54,34, 55,20, 48,18, 51,6
std_cols_petro=10,1, 25,1, 28,1, 36,1, 40,1, 54,1, 75,1, 113,1
std_cols_rdtrain=13,76, 24,77, 63,78, 42,76, 54,77, 39,78, 11,76, 62,77
std_cols_nebula=116,1, 119,1, 122,1, 4,1, 9,1, 24,1, 27,1, 36,1
std_cols_majestic=37,36, 36,36, 40,36, 43,41, 47,41, 51,72, 54,75, 55,84
std_cols_buccanee=2,39, 9,39, 17,1, 21,1, 33,0, 37,0, 41,29, 56,29
std_cols_shamal=1,1
std_cols_fcr900=74,74, 75,13, 87,118, 92,3, 115,118, 25,118, 36,0, 118,118
std_cols_nrg500=3,3, 3,8, 6,25, 7,79, 8,82, 36,105, 39,106, 51,118
std_cols_cement=60,24, 61,27, 65,31, 61,61, 81,35, 62,61, 83,66, 83,64
std_cols_towtruck=1,1, 17,20, 18,20, 22,30, 36,43, 44,51, 52,54
std_cols_fortune=2,39, 9,39, 17,1, 21,1, 33,0, 37,0, 41,29, 56,29
std_cols_cadrona=52,1, 53,1, 66,1, 75,1, 76,1, 81,1, 95,1, 109,1
std_cols_willard=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_forklift=110,1, 111,1, 112,1, 114,1, 119,1, 122,1, 4,1, 13,1
std_cols_tractor=2,35, 36,2, 51,53, 91,2, 11,22, 40,35
std_cols_feltzer=73,1, 74,1, 75,1, 77,1, 79,1, 83,1, 84,1, 91,1
std_cols_remingtn=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_slamvan=3,1, 28,1, 31,1, 55,1, 66,1 97,1, 123,1, 118,1
std_cols_blade=9,1, 12,1, 26,96, 30,96, 32,1, 37,1, 57,96, 71,96
std_cols_freight=1,1
std_cols_streak=1,1
std_cols_vortex=96,67, 86,70, 79,74, 70,86, 61,98, 75,75, 75,91
std_cols_vincent=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_bullet=51,1, 58,8, 60,1, 68,8, 2,1, 13,8, 22,1, 36,8
std_cols_clover=13,118, 24,118, 31,93, 32,92, 45,92, 113,92, 119,113, 122,113
std_cols_sadler=76,8, 32,8, 43,8, 67,8, 11,11, 8,90, 2,2, 83,13
std_cols_firela=3,1
std_cols_hustler=50,1, 47,1, 44,96, 40,96, 39,1, 30,1, 28,96, 9,96
std_cols_intruder=62,37, 78,38, 2,62, 3,87, 2,78, 113,78, 119,62, 7,78
std_cols_primo=122,1, 123,1, 125,1, 10,1, 24,1, 37,1, 55,1, 66,1
std_cols_cargobob=1,1
std_cols_tampa=74,39, 72,39, 75,39, 79,39, 83,36, 84,36, 89,35, 91,35
std_cols_sunrise=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_merit=67,1, 72,1, 75,1, 83,1, 91,1, 101,1, 109,1, 20,1
std_cols_utility=56,56, 49,49, 26,124
std_cols_nevada=38,9, 55,23, 61,74, 71,87, 91,87, 98,114, 102,119, 111,3
std_cols_yosemite=53,32, 15,32, 45,32, 34,30, 65,32, 14,32, 12,32, 43,32
std_cols_windsor=51,1, 58,1, 60,1, 68,1, 2,1, 13,1, 22,1, 36,1
std_cols_monstera=1,1
std_cols_monsterb=1,1
std_cols_uranus=112,1, 116,1, 117,1, 24,1, 30,1, 35,1, 36,1, 40,1
std_cols_jester=51,1, 58,8, 60,1, 68,8, 2,1, 13,8, 22,1, 36,8
std_cols_sultan=52,39, 9,39, 17,1, 21,1, 33,0, 37,0, 41,29, 56,29
std_cols_stratum=57,8, 8,17, 43,21, 54,38, 67,8, 37,78, 65,79, 25,78
std_cols_elegy=36,1, 35,1, 17,1, 11,1, 116,1, 113,1, 101,1, 92,1
std_cols_raindanc=1,6
std_cols_flash=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_tahoma=109,1, 30,8, 95,1, 84,8, 83,1, 72,8, 71,1, 52,8
std_cols_savanna=97,96, 88,64, 90,96, 93,64, 97,96, 99,81, 102,114, 114,1
std_cols_bandito=2,39, 9,39, 17,1, 21,1, 33,0, 37,0, 41,29, 56,29
std_cols_streakc=1,1
std_cols_kart=2,35, 36,2, 51,53, 91,2, 11,22, 40,35
std_cols_mower=94,1, 101,1, 116,1, 117,1, 4,1, 25,1, 30,1, 37,1
std_cols_duneride=91,38, 115,43, 85,6, 79,7, 78,8, 77,18, 79,18, 86,24
std_cols_sweeper=26,26
std_cols_broadway=12,1, 19,96, 31,64, 25,96, 38,1, 51,96, 57,1, 66,96
std_cols_tornado=67,1, 68,96, 72,1, 74,8, 75,96, 76,8, 79,1, 84,96
std_cols_at400=1,3, 8,7, 8,10, 8,16, 23,31, 40,44
std_cols_dft30=1,1
std_cols_huntley=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_stafford=92,92, 81,81, 67,67, 66,66, 61,61, 53,53, 51,51, 47,47, 43,43
std_cols_bf400=54,1, 58,1, 66,1, 72,1, 75,1, 87,1, 101,1, 36,1
std_cols_newsvan=41,10, 41,20, 49,11, 56,123, 110,113, 112,116, 114,118, 119,101
std_cols_tug=1,1
std_cols_petrotr=1,1
std_cols_emperor=37,37, 42,42, 53,53, 62,62, 7,7, 10,10, 11,11, 15,15
std_cols_wayfarer=119,1, 122,1, 8,1, 10,1, 13,1, 25,1, 27,1, 32,1
std_cols_euros=36,1, 40,1, 43,1, 53,1, 72,1, 75,1, 95,1, 101,1
std_cols_hotdog=1,1
std_cols_club=37,37, 31,31, 23,23, 22,22, 7,7, 124,124, 114,114, 112,112
std_cols_artict3=1,1
std_cols_androm=1,1
std_cols_dodo=51,1, 58,8, 60,1, 68,8, 2,1, 13,8, 22,1, 36,8
std_cols_launch=112,20
std_cols_copcarla=7,1
std_cols_copcarsf=7,1
std_cols_copcarvg=7,1
std_cols_copcarru=7,1
std_cols_picador=81,8, 32,8, 43,8, 67,8, 11,11, 8,90, 2,2, 83,13
std_cols_swatvan=1,1
std_cols_alpha=58,1, 69,1, 75,77, 18,1, 32,1, 45,45, 13,1, 34,1
std_cols_phoenix=58,1, 69,1, 75,77, 18,1, 32,1, 45,45, 13,1, 34,1
std_cols_glenshit=67,76, 68,76, 78,76, 2,76, 16,76, 18,76, 25,76, 45,88
std_cols_sadlshit=61,8, 32,8, 43,8, 67,8, 11,11, 8,90, 2,2, 83,13
std_cols_tugstair=1,1
std_cols_boxburg=36,36