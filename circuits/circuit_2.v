// Benchmark "top" written by ABC on Wed Jun 10 20:18:34 2020

module top ( 
    PI_c_15_, PI_c_14_, PI_c_13_, PI_c_12_, PI_c_11_, PI_c_10_, PI_c_9_,
    PI_c_8_, PI_c_7_, PI_c_6_, PI_c_5_, PI_c_4_, PI_c_3_, PI_c_2_, PI_c_1_,
    PI_c_0_, PI_b_15_, PI_b_14_, PI_b_13_, PI_b_12_, PI_b_11_, PI_b_10_,
    PI_b_9_, PI_b_8_, PI_b_7_, PI_b_6_, PI_b_5_, PI_b_4_, PI_b_3_, PI_b_2_,
    PI_b_1_, PI_b_0_, PI_a_15_, PI_a_14_, PI_a_13_, PI_a_12_, PI_a_11_,
    PI_a_10_, PI_a_9_, PI_a_8_, PI_a_7_, PI_a_6_, PI_a_5_, PI_a_4_,
    PI_a_3_, PI_a_2_, PI_a_1_, PI_a_0_, PI_d_2_, PI_d_1_, PI_d_0_, PI_E,
    PI_F,
    PO_out_16_, PO_out_15_, PO_out_14_, PO_out_13_, PO_out_12_, PO_out_11_,
    PO_out_10_, PO_out_9_, PO_out_8_, PO_out_7_, PO_out_6_, PO_out_5_,
    PO_out_4_, PO_out_3_, PO_out_2_, PO_out_1_, PO_out_0_, PO_o1, PO_o2  );
  input  PI_c_15_, PI_c_14_, PI_c_13_, PI_c_12_, PI_c_11_, PI_c_10_,
    PI_c_9_, PI_c_8_, PI_c_7_, PI_c_6_, PI_c_5_, PI_c_4_, PI_c_3_, PI_c_2_,
    PI_c_1_, PI_c_0_, PI_b_15_, PI_b_14_, PI_b_13_, PI_b_12_, PI_b_11_,
    PI_b_10_, PI_b_9_, PI_b_8_, PI_b_7_, PI_b_6_, PI_b_5_, PI_b_4_,
    PI_b_3_, PI_b_2_, PI_b_1_, PI_b_0_, PI_a_15_, PI_a_14_, PI_a_13_,
    PI_a_12_, PI_a_11_, PI_a_10_, PI_a_9_, PI_a_8_, PI_a_7_, PI_a_6_,
    PI_a_5_, PI_a_4_, PI_a_3_, PI_a_2_, PI_a_1_, PI_a_0_, PI_d_2_, PI_d_1_,
    PI_d_0_, PI_E, PI_F;
  output PO_out_16_, PO_out_15_, PO_out_14_, PO_out_13_, PO_out_12_,
    PO_out_11_, PO_out_10_, PO_out_9_, PO_out_8_, PO_out_7_, PO_out_6_,
    PO_out_5_, PO_out_4_, PO_out_3_, PO_out_2_, PO_out_1_, PO_out_0_,
    PO_o1, PO_o2;
  wire new_n73_, new_n74_, new_n75_, new_n76_, new_n77_, new_n78_, new_n79_,
    new_n80_, new_n81_, new_n82_, new_n83_, new_n84_, new_n85_, new_n86_,
    new_n87_, new_n88_, new_n89_, new_n90_, new_n91_, new_n92_, new_n93_,
    new_n94_, new_n95_, new_n96_, new_n97_, new_n98_, new_n99_, new_n100_,
    new_n101_, new_n102_, new_n103_, new_n104_, new_n105_, new_n106_,
    new_n107_, new_n108_, new_n109_, new_n110_, new_n111_, new_n112_,
    new_n113_, new_n114_, new_n115_, new_n116_, new_n117_, new_n118_,
    new_n119_, new_n120_, new_n121_, new_n122_, new_n123_, new_n124_,
    new_n125_, new_n126_, new_n127_, new_n128_, new_n129_, new_n130_,
    new_n131_, new_n132_, new_n133_, new_n134_, new_n135_, new_n136_,
    new_n137_, new_n138_, new_n139_, new_n140_, new_n141_, new_n142_,
    new_n143_, new_n144_, new_n145_, new_n146_, new_n147_, new_n148_,
    new_n149_, new_n150_, new_n151_, new_n152_, new_n153_, new_n154_,
    new_n155_, new_n156_, new_n157_, new_n158_, new_n159_, new_n160_,
    new_n161_, new_n162_, new_n163_, new_n164_, new_n165_, new_n166_,
    new_n167_, new_n168_, new_n169_, new_n170_, new_n171_, new_n172_,
    new_n173_, new_n174_, new_n175_, new_n176_, new_n177_, new_n178_,
    new_n179_, new_n180_, new_n181_, new_n182_, new_n183_, new_n184_,
    new_n185_, new_n186_, new_n187_, new_n188_, new_n189_, new_n190_,
    new_n191_, new_n192_, new_n193_, new_n194_, new_n195_, new_n196_,
    new_n197_, new_n198_, new_n199_, new_n200_, new_n201_, new_n202_,
    new_n203_, new_n204_, new_n205_, new_n206_, new_n207_, new_n208_,
    new_n209_, new_n210_, new_n211_, new_n212_, new_n213_, new_n214_,
    new_n215_, new_n216_, new_n217_, new_n218_, new_n219_, new_n220_,
    new_n221_, new_n222_, new_n223_, new_n224_, new_n225_, new_n226_,
    new_n227_, new_n228_, new_n229_, new_n230_, new_n231_, new_n232_,
    new_n233_, new_n234_, new_n235_, new_n236_, new_n237_, new_n238_,
    new_n239_, new_n240_, new_n241_, new_n242_, new_n243_, new_n244_,
    new_n245_, new_n246_, new_n247_, new_n248_, new_n249_, new_n250_,
    new_n251_, new_n252_, new_n253_, new_n254_, new_n255_, new_n256_,
    new_n257_, new_n258_, new_n259_, new_n260_, new_n261_, new_n262_,
    new_n263_, new_n264_, new_n265_, new_n266_, new_n267_, new_n268_,
    new_n269_, new_n270_, new_n271_, new_n272_, new_n273_, new_n274_,
    new_n275_, new_n276_, new_n277_, new_n278_, new_n279_, new_n280_,
    new_n281_, new_n282_, new_n283_, new_n284_, new_n285_, new_n286_,
    new_n287_, new_n288_;
  not  (new_n73_, PI_c_0_);
  not  (new_n74_, PI_c_1_);
  and  (new_n75_, new_n73_, new_n74_);
  not  (new_n76_, PI_c_2_);
  and  (new_n77_, new_n75_, new_n76_);
  not  (new_n78_, PI_c_3_);
  and  (new_n79_, new_n77_, new_n78_);
  not  (new_n80_, PI_c_4_);
  and  (new_n81_, new_n79_, new_n80_);
  not  (new_n82_, PI_c_5_);
  and  (new_n83_, new_n81_, new_n82_);
  not  (new_n84_, PI_c_6_);
  and  (new_n85_, new_n83_, new_n84_);
  not  (new_n86_, PI_c_7_);
  and  (new_n87_, new_n85_, new_n86_);
  not  (new_n88_, PI_c_8_);
  and  (new_n89_, new_n87_, new_n88_);
  not  (new_n90_, PI_c_9_);
  and  (new_n91_, new_n89_, new_n90_);
  not  (new_n92_, PI_c_10_);
  and  (new_n93_, new_n91_, new_n92_);
  not  (new_n94_, PI_c_11_);
  and  (new_n95_, new_n93_, new_n94_);
  not  (new_n96_, PI_c_12_);
  and  (new_n97_, new_n95_, new_n96_);
  not  (new_n98_, PI_c_13_);
  and  (new_n99_, new_n97_, new_n98_);
  not  (new_n100_, PI_c_14_);
  and  (new_n101_, new_n99_, new_n100_);
  not  (new_n102_, PI_c_15_);
  xor  (new_n103_, new_n101_, new_n102_);
  not  (new_n104_, PI_a_15_);
  xor  (new_n105_, new_n103_, new_n104_);
  not  (new_n106_, new_n105_);
  xor  (new_n107_, new_n99_, new_n100_);
  not  (new_n108_, PI_a_14_);
  xor  (new_n109_, new_n107_, new_n108_);
  xor  (new_n110_, new_n97_, new_n98_);
  not  (new_n111_, PI_a_13_);
  xor  (new_n112_, new_n110_, new_n111_);
  xor  (new_n113_, new_n95_, new_n96_);
  not  (new_n114_, PI_a_12_);
  xor  (new_n115_, new_n113_, new_n114_);
  xor  (new_n116_, new_n93_, new_n94_);
  not  (new_n117_, PI_a_11_);
  xor  (new_n118_, new_n116_, new_n117_);
  xor  (new_n119_, new_n91_, new_n92_);
  not  (new_n120_, PI_a_10_);
  xor  (new_n121_, new_n119_, new_n120_);
  xor  (new_n122_, new_n89_, new_n90_);
  not  (new_n123_, PI_a_9_);
  xor  (new_n124_, new_n122_, new_n123_);
  xor  (new_n125_, new_n87_, new_n88_);
  not  (new_n126_, PI_a_8_);
  xor  (new_n127_, new_n125_, new_n126_);
  xor  (new_n128_, new_n85_, new_n86_);
  not  (new_n129_, PI_a_7_);
  xor  (new_n130_, new_n128_, new_n129_);
  xor  (new_n131_, new_n83_, new_n84_);
  not  (new_n132_, PI_a_6_);
  xor  (new_n133_, new_n131_, new_n132_);
  xor  (new_n134_, new_n81_, new_n82_);
  not  (new_n135_, PI_a_5_);
  xor  (new_n136_, new_n134_, new_n135_);
  xor  (new_n137_, new_n79_, new_n80_);
  not  (new_n138_, PI_a_4_);
  xor  (new_n139_, new_n137_, new_n138_);
  xor  (new_n140_, new_n77_, new_n78_);
  not  (new_n141_, PI_a_3_);
  xor  (new_n142_, new_n140_, new_n141_);
  xor  (new_n143_, new_n75_, new_n76_);
  not  (new_n144_, PI_a_2_);
  xor  (new_n145_, new_n143_, new_n144_);
  xor  (new_n146_, PI_c_0_, PI_c_1_);
  not  (new_n147_, PI_a_1_);
  xor  (new_n148_, new_n146_, new_n147_);
  and  (new_n149_, PI_a_0_, new_n73_);
  and  (new_n150_, new_n148_, new_n149_);
  nor  (new_n151_, new_n146_, new_n147_);
  or   (new_n152_, new_n150_, new_n151_);
  and  (new_n153_, new_n145_, new_n152_);
  nor  (new_n154_, new_n143_, new_n144_);
  or   (new_n155_, new_n153_, new_n154_);
  and  (new_n156_, new_n142_, new_n155_);
  nor  (new_n157_, new_n140_, new_n141_);
  or   (new_n158_, new_n156_, new_n157_);
  and  (new_n159_, new_n139_, new_n158_);
  nor  (new_n160_, new_n137_, new_n138_);
  or   (new_n161_, new_n159_, new_n160_);
  and  (new_n162_, new_n136_, new_n161_);
  nor  (new_n163_, new_n134_, new_n135_);
  or   (new_n164_, new_n162_, new_n163_);
  and  (new_n165_, new_n133_, new_n164_);
  nor  (new_n166_, new_n131_, new_n132_);
  or   (new_n167_, new_n165_, new_n166_);
  and  (new_n168_, new_n130_, new_n167_);
  nor  (new_n169_, new_n128_, new_n129_);
  or   (new_n170_, new_n168_, new_n169_);
  and  (new_n171_, new_n127_, new_n170_);
  nor  (new_n172_, new_n125_, new_n126_);
  or   (new_n173_, new_n171_, new_n172_);
  and  (new_n174_, new_n124_, new_n173_);
  nor  (new_n175_, new_n122_, new_n123_);
  or   (new_n176_, new_n174_, new_n175_);
  and  (new_n177_, new_n121_, new_n176_);
  nor  (new_n178_, new_n119_, new_n120_);
  or   (new_n179_, new_n177_, new_n178_);
  and  (new_n180_, new_n118_, new_n179_);
  nor  (new_n181_, new_n116_, new_n117_);
  or   (new_n182_, new_n180_, new_n181_);
  and  (new_n183_, new_n115_, new_n182_);
  nor  (new_n184_, new_n113_, new_n114_);
  or   (new_n185_, new_n183_, new_n184_);
  and  (new_n186_, new_n112_, new_n185_);
  nor  (new_n187_, new_n110_, new_n111_);
  or   (new_n188_, new_n186_, new_n187_);
  and  (new_n189_, new_n109_, new_n188_);
  not  (new_n190_, new_n189_);
  nor  (new_n191_, new_n107_, new_n108_);
  not  (new_n192_, new_n191_);
  and  (new_n193_, new_n190_, new_n192_);
  or   (new_n194_, new_n106_, new_n193_);
  or   (new_n195_, new_n103_, new_n104_);
  and  (new_n196_, new_n194_, new_n195_);
  not  (new_n197_, new_n101_);
  or   (new_n198_, new_n197_, PI_c_15_);
  xor  (new_n199_, new_n196_, new_n198_);
  xor  (new_n200_, new_n124_, new_n173_);
  xor  (new_n201_, new_n200_, PI_b_9_);
  xor  (new_n202_, new_n127_, new_n170_);
  xor  (new_n203_, new_n202_, PI_b_8_);
  xor  (new_n204_, new_n130_, new_n167_);
  xor  (new_n205_, new_n204_, PI_b_7_);
  xor  (new_n206_, new_n133_, new_n164_);
  xor  (new_n207_, new_n206_, PI_b_6_);
  xor  (new_n208_, new_n136_, new_n161_);
  xor  (new_n209_, new_n208_, PI_b_5_);
  xor  (new_n210_, new_n139_, new_n158_);
  xor  (new_n211_, new_n210_, PI_b_4_);
  xor  (new_n212_, new_n142_, new_n155_);
  xor  (new_n213_, new_n212_, PI_b_3_);
  xor  (new_n214_, new_n145_, new_n152_);
  xor  (new_n215_, new_n214_, PI_b_2_);
  xor  (new_n216_, new_n148_, new_n149_);
  xor  (new_n217_, new_n216_, PI_b_1_);
  xor  (new_n218_, PI_a_0_, new_n73_);
  and  (new_n219_, new_n218_, PI_b_0_);
  and  (new_n220_, new_n217_, new_n219_);
  and  (new_n221_, new_n216_, PI_b_1_);
  or   (new_n222_, new_n220_, new_n221_);
  and  (new_n223_, new_n215_, new_n222_);
  and  (new_n224_, new_n214_, PI_b_2_);
  or   (new_n225_, new_n223_, new_n224_);
  and  (new_n226_, new_n213_, new_n225_);
  and  (new_n227_, new_n212_, PI_b_3_);
  or   (new_n228_, new_n226_, new_n227_);
  and  (new_n229_, new_n211_, new_n228_);
  and  (new_n230_, new_n210_, PI_b_4_);
  or   (new_n231_, new_n229_, new_n230_);
  and  (new_n232_, new_n209_, new_n231_);
  and  (new_n233_, new_n208_, PI_b_5_);
  or   (new_n234_, new_n232_, new_n233_);
  and  (new_n235_, new_n207_, new_n234_);
  and  (new_n236_, new_n206_, PI_b_6_);
  or   (new_n237_, new_n235_, new_n236_);
  and  (new_n238_, new_n205_, new_n237_);
  and  (new_n239_, new_n204_, PI_b_7_);
  or   (new_n240_, new_n238_, new_n239_);
  and  (new_n241_, new_n203_, new_n240_);
  and  (new_n242_, new_n202_, PI_b_8_);
  or   (new_n243_, new_n241_, new_n242_);
  and  (new_n244_, new_n201_, new_n243_);
  not  (new_n245_, new_n244_);
  and  (new_n246_, new_n200_, PI_b_9_);
  not  (new_n247_, new_n246_);
  and  (new_n248_, new_n245_, new_n247_);
  not  (new_n249_, new_n248_);
  xor  (new_n250_, new_n121_, new_n176_);
  not  (new_n251_, PI_b_10_);
  xor  (new_n252_, new_n250_, new_n251_);
  or   (new_n253_, new_n249_, new_n252_);
  or   (new_n254_, new_n250_, PI_b_10_);
  and  (new_n255_, new_n253_, new_n254_);
  xor  (new_n256_, new_n118_, new_n179_);
  xor  (new_n257_, new_n256_, PI_b_11_);
  not  (new_n258_, new_n257_);
  or   (new_n259_, new_n255_, new_n258_);
  or   (new_n260_, new_n256_, PI_b_11_);
  and  (new_n261_, new_n259_, new_n260_);
  xor  (new_n262_, new_n115_, new_n182_);
  xor  (new_n263_, new_n262_, PI_b_12_);
  nand (new_n264_, new_n261_, new_n263_);
  nand (new_n265_, new_n262_, PI_b_12_);
  and  (new_n266_, new_n264_, new_n265_);
  xor  (new_n267_, new_n112_, new_n185_);
  not  (new_n268_, PI_b_13_);
  xor  (new_n269_, new_n267_, new_n268_);
  nor  (new_n270_, new_n266_, new_n269_);
  not  (new_n271_, new_n270_);
  and  (new_n272_, new_n267_, PI_b_13_);
  not  (new_n273_, new_n272_);
  and  (new_n274_, new_n271_, new_n273_);
  not  (new_n275_, new_n274_);
  xor  (new_n276_, new_n109_, new_n188_);
  not  (new_n277_, PI_b_14_);
  xor  (new_n278_, new_n276_, new_n277_);
  or   (new_n279_, new_n275_, new_n278_);
  or   (new_n280_, new_n276_, PI_b_14_);
  and  (new_n281_, new_n279_, new_n280_);
  not  (new_n282_, new_n193_);
  xor  (new_n283_, new_n105_, new_n282_);
  xor  (new_n284_, new_n283_, PI_b_15_);
  not  (new_n285_, new_n284_);
  or   (new_n286_, new_n281_, new_n285_);
  or   (new_n287_, new_n283_, PI_b_15_);
  and  (new_n288_, new_n286_, new_n287_);
  xor  (PO_out_16_, new_n199_, new_n288_);
  xor  (PO_out_15_, new_n281_, new_n284_);
  xor  (PO_out_14_, new_n274_, new_n278_);
  xor  (PO_out_13_, new_n266_, new_n269_);
  xor  (PO_out_12_, new_n261_, new_n263_);
  xor  (PO_out_11_, new_n255_, new_n257_);
  xor  (PO_out_10_, new_n248_, new_n252_);
  xor  (PO_out_9_, new_n201_, new_n243_);
  xor  (PO_out_8_, new_n203_, new_n240_);
  xor  (PO_out_7_, new_n205_, new_n237_);
  xor  (PO_out_6_, new_n207_, new_n234_);
  xor  (PO_out_5_, new_n209_, new_n231_);
  xor  (PO_out_4_, new_n211_, new_n228_);
  xor  (PO_out_3_, new_n213_, new_n225_);
  xor  (PO_out_2_, new_n215_, new_n222_);
  xor  (PO_out_1_, new_n217_, new_n219_);
  xor  (PO_out_0_, new_n218_, PI_b_0_);
  not  (PO_o1, PI_F);
  buf (PO_o2, 1'b1);
endmodule


