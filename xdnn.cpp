#include <stdio.h>
#include <ap_int.h>

#define BITS_PER_VAL 2
#define DEPTH 16

inline ap_int<BITS_PER_VAL> get_binval(ap_int<512> map, int index){
	#pragma HLS INLINE
	return map.range((index+1)*BITS_PER_VAL-1, index*BITS_PER_VAL);
}

inline void put_binval(ap_int<512> *map, ap_int<BITS_PER_VAL> val, int index){
	#pragma HLS INLINE
	map->range((index+1)*BITS_PER_VAL-1, index*BITS_PER_VAL) = val;
}

inline void custom_memcpy(void *to, void *from, int size){
	#pragma HLS INLINE
	char *sptr = (char *)from;
	char *dptr = (char *)to;
	for (int i = 0; i < size; i++){
		#pragma HLS PIPELINE II=1
		dptr[i] = sptr[i];
	}
}

void xbnn(ap_int<512> *ifmap, ap_int<512> *kmap, ap_int<512> *ofmap){
	#pragma HLS INTERFACE m_axi depth=16 port=ifmap offset=slave bundle=gmem0
	#pragma HLS INTERFACE m_axi depth=16 port=kmap offset=slave bundle=gmem1
	#pragma HLS INTERFACE m_axi depth=16 port=ofmap offset=slave bundle=gmem0

	#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
	#pragma HLS INTERFACE s_axilite port=kmap bundle=control
	#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
	#pragma HLS INTERFACE s_axilite port=return bundle=control

	ap_int<512> aa[DEPTH], bb[DEPTH], cc[DEPTH];
	{
		#pragma HLS LOOP MERGE
		memcpy(&aa, ifmap, 512*DEPTH/8);
		memcpy(&bb, kmap, 512*DEPTH/8);
	}
	mul_loop: for (int i = 0; i < 256; i++){
	for (int j = 0; j < 16; j++){
			#pragma HLS PIPELINE II=1
			put_binval(&cc[j], ap_int<BITS_PER_VAL>(get_binval(aa[j], i)*get_binval(bb[j], i)), i);
		}
	}
	memcpy(ofmap, &cc, 512*DEPTH/8);
}
