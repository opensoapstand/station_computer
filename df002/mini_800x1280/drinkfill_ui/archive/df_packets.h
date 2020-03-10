#ifndef DF_QUERIES_H
#define DF_QUERIES_H

#endif // DF_QUERIES_H
enum class query_types {
    q_number_products ,
    r_number_products,
    q_product_data,
    r_product_data,
    s_product_data,
    q_product_avail,
    r_product_avail,
    s_product_avail,
    q_region_data,
    r_region_data,
    s_region_data,
    q_dispense,
    r_dispense,
    s_dispense,
    q_error,
    r_error,
    q_state,
    r_state,
    s_state,
    d_state  // dummy state

};

enum class df_State {
    init,
    idle,
    selected,
    payment,
    dispense,
    dispensePaused,
    dispenseErr,
    dispenseEnded,
    updateInven,
    maintenance,
    error
};

struct packet {
	 int version ;
	 int id ;
	 int body ;
	 query_types query ;
	 df_State stateToSet;

} ;
