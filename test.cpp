#include <stdio.h>
#include <ap_int.h>

#define BITS_PER_VAL 32

inline ap_int<BITS_PER_VAL> get_binval(ap_int<512> map, int index){
	#pragma HLS INLINE
	return map.range((index+1)*BITS_PER_VAL-1, index*BITS_PER_VAL);
}

inline void put_binval(ap_int<512> *map, ap_int<BITS_PER_VAL> val, int index){
	#pragma HLS INLINE
	map->range((index+1)*BITS_PER_VAL-1, index*BITS_PER_VAL) = val;
}

void xbnn(ap_int<512> *ifmap, ap_int<512> *kmap, ap_int<512> *ofmap){
	#pragma HLS INTERFACE m_axi depth=1 port=ifmap offset=slave bundle=gmem0
	#pragma HLS INTERFACE m_axi depth=1 port=kmap offset=slave bundle=gmem1
	#pragma HLS INTERFACE m_axi depth=1 port=ofmap offset=slave bundle=gmem0

	#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
	#pragma HLS INTERFACE s_axilite port=kmap bundle=control
	#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
	#pragma HLS INTERFACE s_axilite port=return bundle=control

	ap_int<512> aa, bb, cc;
	{
		#pragma HLS LOOP MERGE
		memcpy(&aa, ifmap, 64);
		memcpy(&bb, kmap, 64);
	}

	mul_loop: for (int i = 0; i < 512/BITS_PER_VAL; i++){
		#pragma HLS PIPELINE II=1
		put_binval(&cc, ap_int<BITS_PER_VAL>(get_binval(aa, i)*get_binval(bb, i)), i);
	}
	memcpy(ofmap, &cc, 64);
}

