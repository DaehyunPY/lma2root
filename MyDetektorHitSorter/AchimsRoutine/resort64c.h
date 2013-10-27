
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RESORT64C_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RESORT64C_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

//#define LINUX


#ifndef LINUX
	#ifndef WINVER
		#define WINVER 0x0501
	#endif
	#pragma warning(disable : 4005)
#endif

#ifndef NUM_IONS
	#define NUM_IONS (100)  // needed for users who are porting their old projects from the old sort routine to the new one
#endif

#ifndef RESORT_IS_ALREADY_DEFINED
#define RESORT_IS_ALREADY_DEFINED

#ifndef LINUX
	#ifdef RESORT64C_EXPORTS
		#define RESORT64C_API __declspec(dllexport)
	#else
		#define RESORT64C_API __declspec(dllimport)
	#endif

	#ifdef NO_RESORT64C_DLL_IMPORT
		#define RESORT64C_API
	#endif
#else
	#define __int32 int
	#define __int16 short
	#define RESORT64C_API
#endif


//	List of error_codes of init()-function:
// -1  wrong channel numbers
// -2  uncorrected_time_sum_half_widths not correctly initialized
// -3  corrected_time_sum_half_widths not correctly initialized
// -4  tdc_array_row_length not correctly initialized
// -5  MCP_radius not correctly initialized
// -6  runtime not correctly initialized
// -7  scalefactors not correctly initialized
// -8  error allocating memory for internal arrays
// -9  error during initialization of sum correctors
// -10 error during initialization of pos correctors
// -11 address of count-array not given
// -12 address of tdc-array not given



class sort_class;
class profile_class;
class gaussfit_class;
class hit_class;
class interpolate_class;
class sum_walk_calibration_class;
class scalefactors_calibration_class;
class nonlinearity_calibration_class;
#ifdef USE_DOUBLE_ARRAY_CLASS
class double_array_class;
#endif




#ifdef USE_DOUBLE_ARRAY_CLASS
class RESORT64C_API double_array_class {
public:
   double_array_class();   // CONSTRUCTOR
   ~double_array_class();
   double& operator[]( __int32 nSubscript );
   double *pointer;
   __int32 * pointer_to_cnt;
};





class RESORT64C_API double_array_pointer_class {
public:
   double_array_pointer_class();  // CONSTRUCTOR
   ~double_array_pointer_class();
   double_array_class& operator[]( __int32 nSubscript );
   double_array_class pointer[7];
};
#endif










class RESORT64C_API version_number_class
{
public:
	__int32 A;
	__int32 B;
	__int32 C;
	__int32 D;
};










class RESORT64C_API nonlinearity_calibration_class
{
public:
	static nonlinearity_calibration_class * new_nonlinearity_calibration_class(sort_class* mother_sort_instance, __int32 number_of_stages, __int32 number_of_columns, double fu, double fv, double fw);  // CONSTRUCTOR
	virtual ~nonlinearity_calibration_class();		// FUNCTION

	virtual void finalize();						// FUNCTION
	virtual bool init(sort_class * mother_sort_instance);				// FUNCTION
	virtual void fill_pos_histograms();				// FUNCTION
	virtual void generate_pos_correction_profiles();	// FUNCTION
//	virtual nonlinearity_calibration_class * clone();	// FUNCTION

	sort_class * sorter;

	double	fu,fv,fw;
	__int32	number_of_columns;

	profile_class ** pos_profile[3];

	bool initialization_successful;

	double tdc_resolution;

	__int32 number_of_stages;

private:
	nonlinearity_calibration_class(sort_class* mother_sort_instance, __int32 number_of_stages, __int32 number_of_columns, double fu, double fv, double fw);  // FUNCTION
	float * stack[6];
	__int32		stack_counter;
	__int32		stack_size;
};




















class RESORT64C_API sum_walk_calibration_class
{

public:
	static sum_walk_calibration_class * new_sum_walk_calibration_class(sort_class* mother_sort_instance, __int32 number_of_columns);  // CONSTRUCTOR
	virtual ~sum_walk_calibration_class();			// FUNCTION
	virtual void finalize();						// FUNCTION
	virtual bool init(sort_class * mother_sort_instance);				// FUNCTION
	virtual void fill_sum_histograms();				// FUNCTION
	virtual void generate_sum_walk_profiles();		// FUNCTION
	virtual sum_walk_calibration_class * clone();	// FUNCTION

	sort_class * sorter;

	__int32 number_of_columns;

	profile_class * sumu_profile;
	profile_class * sumv_profile;
	profile_class * sumw_profile;

	bool initialization_successful;

	double	tdc_resolution;

private:
	sum_walk_calibration_class(sort_class* mother_sort_instance, __int32 number_of_columns);  // FUNCTION
};










class RESORT64C_API scalefactors_calibration_class
{

public:
	
	scalefactors_calibration_class(bool BuildStack, double smaller_runtime_limit, double fu,double fv,double fw);  // CONSTRUCTOR
	virtual ~scalefactors_calibration_class();  // FUNCTION

	virtual void feed_calibration_data(double u_ns, double v_ns, double w_ns, double w_ns_minus_woffset);  // FUNCTION
	virtual void do_auto_calibration(double w_offset);  // FUNCTION
	virtual void finalize();  // FUNCTION
	
	virtual scalefactors_calibration_class * clone();  // FUNCTION

	double	fu,fv,fw;

	double	best_fv, best_fw, best_w_offset;

	double	detector_map_fill;
	__int32	binx,biny;
	__int32 detector_map_size;


private:
	virtual double calibrate(double fv, double fw, double w_offset);  // FUNCTION

	bool	BuildStack;
	double	runtime_limit;
	float * u_ns_stack;
	float * v_ns_stack;
	float * w_ns_stack;
	float * X_calibration;
	float * Y_calibration;
	__int16	**detector_map_counter;
	__int32		TDC_stack_counter;
	__int32 tdc_stack_size;
	
};










class RESORT64C_API gaussfit_class
{

public:
	gaussfit_class();// CONSTRUCTOR
	gaussfit_class(double * double_y_array,__int32 number_of_points);// CONSTRUCTOR
	gaussfit_class(__int32 * int_y_array,__int32 number_of_points);// CONSTRUCTOR
	gaussfit_class(double * double_y_array,double fwhm,double x_pos,double height,__int32 number_of_points);// CONSTRUCTOR
	gaussfit_class(__int32 * int_y_array,double fwhm,double x_pos,double height,__int32 number_of_points);// CONSTRUCTOR
	virtual ~gaussfit_class();



	virtual bool do_gauss_fit();  // FUNCTION
	virtual bool find_good_start_values();  // FUNCTION
	virtual void init();  // FUNCTION
	virtual void reset();  // FUNCTION
	virtual void smooth();  // FUNCTION
	virtual	gaussfit_class * clone();  // FUNCTION

	virtual double get_random_gauss_value();  // FUNCTION
	virtual double get_random_value();  // FUNCTION

	__int32		number_of_points;
	double*		double_y_array;
	double		fwhm,height,x_pos;
	double		fwhm_stepsize,height_stepsize,x_pos_stepsize;
	__int32		max_number_of_iterations;

private:

	bool private_array_used;
	virtual void copy_to_private_double_array(__int32 *);  // FUNCTION
	virtual void copy_to_private_double_array(double *);  // FUNCTION
	virtual double calculate_error(double, double, double);  // FUNCTION
	virtual double gauss_value(double , double , double , double);  // FUNCTION
};









class RESORT64C_API profile_class
{

public:
	profile_class();			// CONSTRUCTOR
	virtual ~profile_class();

	virtual void generate_profile_with_gauss();  // FUNCTION
	virtual void generate_profile_with_box();  // FUNCTION
	virtual bool box_fit(double * fit_array, __int32 number_of_bins, double &pos, double &width); // FUNCTION
	virtual void init();  // FUNCTION
	virtual void reset();  // FUNCTION
	virtual void fill_matrix(double x_pos,double y_pos,double value);  // FUNCTION
	virtual void fill_matrix(__int32 x_bin,__int32 y_bin,double value);  // FUNCTION
	virtual void fill_matrix(double x_pos,double y_pos);  // FUNCTION
	virtual void fill_matrix(__int32 x_bin,__int32 y_bin);  // FUNCTION
	virtual void clear_matrix();  // FUNCTION
	virtual double	 get_x_bin_as_double(double x_pos);  // FUNCTION
	virtual double	 get_y_bin_as_double(double y_pos);  // FUNCTION
	virtual __int32	 get_x_bin(double x_pos);  // FUNCTION
	virtual __int32	 get_y_bin(double y_pos);  // FUNCTION

	virtual double get_bin_center_x(double x_bin);  // FUNCTION
	virtual double get_bin_center_y(double y_bin);  // FUNCTION

	virtual double get_y(__int32 bin);  // FUNCTION
	virtual double get_fwhm(__int32 bin);  // FUNCTION
	virtual double get_y(double x_pos);		// FUNCTION
	virtual double get_fwhm(double x_pos);	// FUNCTION
	virtual unsigned __int32 get_bin_content(__int32 bin);	// FUNCTION
	virtual unsigned __int32 get_bin_content(double  x_pos);	// FUNCTION
	virtual void heal_gaps_where_fit_did_not_work(); // FUNCTION
	virtual profile_class * clone();  // FUNCTION
	

	virtual void transfer_to_interpolate_class_instance(interpolate_class * interpolate_class_instance); // FUNCTION

	__int32 number_of_columns;
	__int32 number_of_rows;
	
	double center_of_upper_bin;
	double center_of_lower_bin;
	double center_of_left_bin;
	double center_of_right_bin;

	double * double_profile_y;
	double * double_profile_fwhm;
	bool   * bool_profile_fit_worked;
	double ** double_source_matrix;
	unsigned __int32 * bin_content;

private:
	double dNaN;
	bool using_internal_matrix;
	bool profile_exists;

};












class RESORT64C_API interpolate_class
{

	friend class sort_class;


public:
	
	interpolate_class();									// CONSTRUCTOR
	virtual ~interpolate_class();

	virtual double	get_y(double x_pos);					// FUNCTION
	void			init();									// FUNCTION
	void			set_point(double x_pos, double y_pos);  // FUNCTION
	void			reset();								// FUNCTION
	virtual			interpolate_class * clone();			// FUNCTION

	double			min_x,max_x;
	
	__int32			number_of_bins_per_nanosecond;
	
	__int32			number_of_points;
	double			binsize_ns;
	bool			is_sorted;

	bool			use_slow_interpolation;

private:
	double			interpolate(double x_pos);  // FUNCTION
	void			sort();  // FUNCTION
	void			find_duplicates();  // FUNCTION
	bool			init_successful;
	__int32 		size_of_array;
	__int32 		size_of_input_arrays;
	double*			x_input;
	double*			y_input;
	double* 		y_pos;
};









class RESORT64C_API hit_class
{

	friend class sort_class;



public:
	
	hit_class(sort_class *);	// CONSTRUCTOR
	virtual ~hit_class();

	

	__int32		iCu1,iCu2,iCv1,iCv2,iCw1,iCw2,iCmcp;	

	double	fu,fv,fw;

	double	uncorrected_time_sum_half_width_u,uncorrected_time_sum_half_width_v,uncorrected_time_sum_half_width_w;
	double	corrected_time_sum_half_width_u,corrected_time_sum_half_width_v,corrected_time_sum_half_width_w;

	__int32		number_of_reconstructed_signals_in_deadtime_shadow;

	double	tu1, tu2;
	double	tv1, tv2;
	double	tw1, tw2;
	double	tmcp;

	double	tsummcp;
	bool	tsummcp_exists;

	double	x, y, time;

	bool	use_HEX;

	__int32		set_of_rules;

	sort_class * sort_instance;
	bool	common_start_mode;

#ifdef USE_DOUBLE_ARRAY_CLASS
	double_array_pointer_class tdc;
#else
	double	*tdc[7];
#endif

	__int32		*cnt;

	__int32		number_of_possible_reflections;
	__int32		number_of_reconstructed_signals_sitting_on_top_of_a_real_signal;
	//__int32 number_of_already_used_signals;

	bool	hit_is_good;
	__int32		number_of_combination;

	__int16	* used_signals[7];
	double	* reconstructed_signals[7];
	__int32		reconstructed_cnt[7];

	__int32		number_of_original_anode_signals;
	bool	with_mcp;

	unsigned __int32 DEBUG_event_number;
	unsigned __int32 unique_hit_identifier;

	__int32		method;

	__int32		sum_stack_size;
	double	*u_sum_stack1, *v_sum_stack1, *w_sum_stack1;
	double	*u_sum_stack2, *v_sum_stack2, *w_sum_stack2;
	__int32		*u_sum_valid, *v_sum_valid, *w_sum_valid;
	__int32		*sum_killers;

	__int32		number_of_destroyed_sums;

	bool	hit_was_treated_by_stolen_signal_function;
	__int32		channel_treated_by_stolen_signal_function;

#ifdef USE_DOUBLE_ARRAY_CLASS
	double_array_class	tdc_Cu1,tdc_Cu2,tdc_Cv1,tdc_Cv2,tdc_Cw1,tdc_Cw2,tdc_Cmcp;
#else
	double	*tdc_Cu1,*tdc_Cu2,*tdc_Cv1,*tdc_Cv2,*tdc_Cw1,*tdc_Cw2,*tdc_Cmcp;
#endif
	__int16	*used_signals_Cu1,*used_signals_Cu2,*used_signals_Cv1,*used_signals_Cv2,*used_signals_Cw1,*used_signals_Cw2,*used_signals_Cmcp;

private:
	virtual void generate(__int32 iCu1, __int32 iCu2, __int32 iCv1, __int32 iCv2, __int32 iCw1, __int32 iCw2, __int32 iCmcp, __int32 number_of_combination, __int32 unique_hit_identifier,__int32 set_of_rules);  // FUNCTION
	virtual bool is_in_deadtime_shadow(double , __int32);  // FUNCTION
	virtual bool is_closer_to_a_real_signal_than(double , __int32 , double); // FUNCTION
	virtual bool could_be_a_reflection(__int32 , __int32);  // FUNCTION
	virtual __int32 get_number_of_signals_which_are_used_more_than(__int32); // FUNCTION
	virtual bool are_N_signals_used_more_than(__int32,__int32); // (including own signals of this hit) FUNCTION
	virtual void clone(hit_class *); // FUNCTION
	virtual void apply_filter_rules(); // FUNCTION
	virtual __int32 get_method_number(__int32,__int32,__int32,__int32,__int32,__int32,__int32); // FUNCTION
	virtual __int32 how_many_reconstructed_pairs_at_MCP_edge(); // FUNCTION
	virtual __int32 get_number_of_destroyed_sums();  // FUNCTION
	virtual bool check_if_a_used_anode_signal_belongs_to_a_destroyed_sum();  // FUNCTION
	virtual void compute_time();  // FUNCTION
	virtual bool signal_is_distorted(__int32,__int32,double,double);  // FUNCTION
};




















class RESORT64C_API sort_class
{

public:
    
	sort_class();			// CONSTRUCTOR
    virtual ~sort_class();

	virtual __int32	sort();  // FUNCTION
	virtual __int32 run_without_sorting();  // FUNCTION
	
	virtual void sort_an_hit_array_in_time(hit_class* hit_array[], __int32 number_of_hits, bool small_numbers_first);  // FUNCTION
//	virtual bool expand_an_hit_array(hit_class**[], __int32 * , __int32); // FUNCTION
	
	virtual __int32 init(); // FUNCTION
	virtual void get_error_text(__int32 error_code,__int32 buffer_length,char* destination_array);

	virtual double correct_sum(double t1, double t2,__int32 number_of_layer);  // FUNCTION
	virtual double correct_pos(double t1, double t2,__int32 number_of_layer);  // FUNCTION
	virtual double get_corrected_anode_signal(double ttmcp, double t,__int32 left_or_right_side, __int32 layer);  // FUNCTION

	virtual void shift_layer_w(__int32 add_or_sub_sign,double w_offset);  // FUNCTION
	virtual void shift_sums(__int32 add_or_sub_sign,double sumu_offset,double sumv_offset);  // FUNCTION
	virtual void shift_sums(__int32 add_or_sub_sign,double sumu_offset,double sumv_offset,double sumw_offset);  // FUNCTION
	virtual void shift_position_origin(__int32 add_or_sub_sign,double pos_x_mm_offset,double pos_y_mm_offset);  // FUNCTION
	virtual sort_class * clone();  // FUNCTION

	version_number_class get_version_number(); // FUNCTION

	// the following variables must be initialized by the user:

	__int32		*count;
	double	* tdc_pointer;
	double	fu,fv,fw;

	double	runtime_u,runtime_v,runtime_w;
	//or
	double	max_runtime;

	__int32		tdc_array_row_length;

#ifdef USE_DOUBLE_ARRAY_CLASS
	double_array_pointer_class tdc;
#else
	double	*tdc[7];
#endif

	__int32		cnt[7];
	double	uncorrected_time_sum_half_width_u,uncorrected_time_sum_half_width_v,uncorrected_time_sum_half_width_w;
	bool	common_start_mode;  // true = common start, false = common stop
	double	MCP_radius;
	bool	use_HEX;
	bool	use_MCP;
	double  TDC_resolution_ns;
	__int32		Cu1,Cu2,Cv1,Cv2,Cw1,Cw2,Cmcp;




	//optional:
	double	corrected_time_sum_half_width_u,corrected_time_sum_half_width_v,corrected_time_sum_half_width_w;
	double	dead_time_anode;
	double	dead_time_mcp;
	bool	use_sum_correction;
	bool	use_pos_correction;
	bool	run_without_sorting_flag;
	bool	dont_overwrite_original_data;

	bool	use_reflection_filter_on_u1,use_reflection_filter_on_u2;
	bool	use_reflection_filter_on_v1,use_reflection_filter_on_v2;
	bool	use_reflection_filter_on_w1,use_reflection_filter_on_w2;

	double  u1_reflection_time_position, u1_reflection_half_width_at_base;
	double  u2_reflection_time_position, u2_reflection_half_width_at_base;
	double  v1_reflection_time_position, v1_reflection_half_width_at_base;
	double  v2_reflection_time_position, v2_reflection_half_width_at_base;
	double  w1_reflection_time_position, w1_reflection_half_width_at_base;
	double  w2_reflection_time_position, w2_reflection_half_width_at_base;


	//the following variables should not be modified:

	double	pos_check_radius_mm;
	double	triple_MCP_time_tolerance_ns;
	double		lower_dead_time_margin;

	__int32		maximum_uses_of_signals;

	//double	pre_dead_time;
	__int32		maximum_of_already_used_signals_in_one_hit;

	

	
	bool	initialization_successful;
	
	double  * reconstructed_signals[7];
	__int32		reconstructed_cnt[7];
	__int16	* used_signals[7];

	__int32		output_hit_array_counter;
	hit_class ** output_hit_array;
	
	unsigned __int32 unique_hit_identifier;

	interpolate_class * sum_corrector_U;
	interpolate_class * sum_corrector_V;
	interpolate_class * sum_corrector_W;

	interpolate_class * pos_corrector_U;
	interpolate_class * pos_corrector_V;
	interpolate_class * pos_corrector_W;


	__int32		DEBUG_flag;
	unsigned __int32 DEBUG_event_number;

	version_number_class version_number;
	
#ifdef USE_DOUBLE_ARRAY_CLASS
	double_array_class	tdc_Cu1,tdc_Cu2,tdc_Cv1,tdc_Cv2,tdc_Cw1,tdc_Cw2,tdc_Cmcp;
#else
	double	*tdc_Cu1,*tdc_Cu2,*tdc_Cv1,*tdc_Cv2,*tdc_Cw1,*tdc_Cw2,*tdc_Cmcp;
#endif

	__int16	*used_signals_Cu1,*used_signals_Cu2,*used_signals_Cv1,*used_signals_Cv2,*used_signals_Cw1,*used_signals_Cw2,*used_signals_Cmcp;
	
	__int32		 local_sort_instance_id;
	

private:

	virtual bool create_new_hit_instance(__int32 &,__int32 &, hit_class **&,__int32 &); // FUNCTION
	virtual bool detect_twins(); // FUNCTION
	virtual bool get_combination(__int32 , __int32 , bool);   // FUNCTION
	virtual void search_combinations(__int32,bool,__int32,__int32,__int32);   // FUNCTION
	virtual bool init_arrays();  // FUNCTION
	virtual void delete_arrays();  // FUNCTION
	virtual void sort_into_output_hit_array();  // FUNCTION
	virtual void get_all_from_one_combination(__int32,__int32,__int32);  // FUNTCION
	virtual bool garbage_was_found();  // FUNTCION
	virtual __int32 find_next_hit_by_signal(__int32, __int32, __int32, hit_class * [], __int32);  // FUNTCION
	virtual void remove_hit_from_output_hit_array(int);  // FUNTCION
	virtual void kill_all_hits_using_this_signal_except(__int32,__int32,__int32*);  // FUNTCION
	virtual void find_stolen_anode_signals(hit_class*[],__int32); // FUNCTION

	
	static __int32 global_instance_id_counter;

	bool	ready_to_run;
	__int32		number_of_already_used_signals;
	double   tdc_signal_dummy;
	__int32		number_of_blocked_combinations;
	unsigned __int32 * blocked_combinations[7];
	__int32		temp_hit_array_counter;
	hit_class ** temp_hit_array;
	__int32		number_of_hit_instances_in_temp_hit_array_stack;
	__int32		number_of_hit_instances_in_output_hit_array_stack;

	__int32		combination[7];

	__int32		size_of_temp_hit_array_stack;
	__int32		size_of_output_hit_array_stack;


};

#endif
