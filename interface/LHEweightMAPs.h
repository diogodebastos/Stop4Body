
std::map<int,std::string>& getLHEMap(int);

std::map<int,std::string>& getNanoAODQ2ScaleMap();

std::map<int,std::string> emptyMap = {};

std::map<int,std::string> LHEweightMAPnanoAOD = {
  {4, "Q2_0"},
  {5, "Q2_1"},
  {3, "Q2_2"},
  {7, "Q2_3"},
  {8, "Q2_4"},
  {6, "Q2_5"},
  {1, "Q2_6"},
  {2, "Q2_7"},
  {0, "Q2_8"}
};

std::map<int,std::string> LHEweightMAP446 = {
  {1, "Q2_0"},
  {2, "Q2_1"},
  {3, "Q2_2"},
  {4, "Q2_3"},
  {5, "Q2_4"},
  {6, "Q2_5"},
  {7, "Q2_6"},
  {8, "Q2_7"},
  {9, "Q2_8"},

  {10,  "NNPDF30_lo_as_0130_0"},
  {11,  "NNPDF30_lo_as_0130_1"},
  {12,  "NNPDF30_lo_as_0130_2"},
  {13,  "NNPDF30_lo_as_0130_3"},
  {14,  "NNPDF30_lo_as_0130_4"},
  {15,  "NNPDF30_lo_as_0130_5"},
  {16,  "NNPDF30_lo_as_0130_6"},
  {17,  "NNPDF30_lo_as_0130_7"},
  {18,  "NNPDF30_lo_as_0130_8"},
  {19,  "NNPDF30_lo_as_0130_9"},
  {20,  "NNPDF30_lo_as_0130_10"},
  {21,  "NNPDF30_lo_as_0130_11"},
  {22,  "NNPDF30_lo_as_0130_12"},
  {23,  "NNPDF30_lo_as_0130_13"},
  {24,  "NNPDF30_lo_as_0130_14"},
  {25,  "NNPDF30_lo_as_0130_15"},
  {26,  "NNPDF30_lo_as_0130_16"},
  {27,  "NNPDF30_lo_as_0130_17"},
  {28,  "NNPDF30_lo_as_0130_18"},
  {29,  "NNPDF30_lo_as_0130_19"},
  {30,  "NNPDF30_lo_as_0130_20"},
  {31,  "NNPDF30_lo_as_0130_21"},
  {32,  "NNPDF30_lo_as_0130_22"},
  {33,  "NNPDF30_lo_as_0130_23"},
  {34,  "NNPDF30_lo_as_0130_24"},
  {35,  "NNPDF30_lo_as_0130_25"},
  {36,  "NNPDF30_lo_as_0130_26"},
  {37,  "NNPDF30_lo_as_0130_27"},
  {38,  "NNPDF30_lo_as_0130_28"},
  {39,  "NNPDF30_lo_as_0130_29"},
  {40,  "NNPDF30_lo_as_0130_30"},
  {41,  "NNPDF30_lo_as_0130_31"},
  {42,  "NNPDF30_lo_as_0130_32"},
  {43,  "NNPDF30_lo_as_0130_33"},
  {44,  "NNPDF30_lo_as_0130_34"},
  {45,  "NNPDF30_lo_as_0130_35"},
  {46,  "NNPDF30_lo_as_0130_36"},
  {47,  "NNPDF30_lo_as_0130_37"},
  {48,  "NNPDF30_lo_as_0130_38"},
  {49,  "NNPDF30_lo_as_0130_39"},
  {50,  "NNPDF30_lo_as_0130_40"},
  {51,  "NNPDF30_lo_as_0130_41"},
  {52,  "NNPDF30_lo_as_0130_42"},
  {53,  "NNPDF30_lo_as_0130_43"},
  {54,  "NNPDF30_lo_as_0130_44"},
  {55,  "NNPDF30_lo_as_0130_45"},
  {56,  "NNPDF30_lo_as_0130_46"},
  {57,  "NNPDF30_lo_as_0130_47"},
  {58,  "NNPDF30_lo_as_0130_48"},
  {59,  "NNPDF30_lo_as_0130_49"},
  {60,  "NNPDF30_lo_as_0130_50"},
  {61,  "NNPDF30_lo_as_0130_51"},
  {62,  "NNPDF30_lo_as_0130_52"},
  {63,  "NNPDF30_lo_as_0130_53"},
  {64,  "NNPDF30_lo_as_0130_54"},
  {65,  "NNPDF30_lo_as_0130_55"},
  {66,  "NNPDF30_lo_as_0130_56"},
  {67,  "NNPDF30_lo_as_0130_57"},
  {68,  "NNPDF30_lo_as_0130_58"},
  {69,  "NNPDF30_lo_as_0130_59"},
  {70,  "NNPDF30_lo_as_0130_60"},
  {71,  "NNPDF30_lo_as_0130_61"},
  {72,  "NNPDF30_lo_as_0130_62"},
  {73,  "NNPDF30_lo_as_0130_63"},
  {74,  "NNPDF30_lo_as_0130_64"},
  {75,  "NNPDF30_lo_as_0130_65"},
  {76,  "NNPDF30_lo_as_0130_66"},
  {77,  "NNPDF30_lo_as_0130_67"},
  {78,  "NNPDF30_lo_as_0130_68"},
  {79,  "NNPDF30_lo_as_0130_69"},
  {80,  "NNPDF30_lo_as_0130_70"},
  {81,  "NNPDF30_lo_as_0130_71"},
  {82,  "NNPDF30_lo_as_0130_72"},
  {83,  "NNPDF30_lo_as_0130_73"},
  {84,  "NNPDF30_lo_as_0130_74"},
  {85,  "NNPDF30_lo_as_0130_75"},
  {86,  "NNPDF30_lo_as_0130_76"},
  {87,  "NNPDF30_lo_as_0130_77"},
  {88,  "NNPDF30_lo_as_0130_78"},
  {89,  "NNPDF30_lo_as_0130_79"},
  {90,  "NNPDF30_lo_as_0130_80"},
  {91,  "NNPDF30_lo_as_0130_81"},
  {92,  "NNPDF30_lo_as_0130_82"},
  {93,  "NNPDF30_lo_as_0130_83"},
  {94,  "NNPDF30_lo_as_0130_84"},
  {95,  "NNPDF30_lo_as_0130_85"},
  {96,  "NNPDF30_lo_as_0130_86"},
  {97,  "NNPDF30_lo_as_0130_87"},
  {98,  "NNPDF30_lo_as_0130_88"},
  {99,  "NNPDF30_lo_as_0130_89"},
  {100, "NNPDF30_lo_as_0130_90"},
  {101, "NNPDF30_lo_as_0130_91"},
  {102, "NNPDF30_lo_as_0130_92"},
  {103, "NNPDF30_lo_as_0130_93"},
  {104, "NNPDF30_lo_as_0130_94"},
  {105, "NNPDF30_lo_as_0130_95"},
  {106, "NNPDF30_lo_as_0130_96"},
  {107, "NNPDF30_lo_as_0130_97"},
  {108, "NNPDF30_lo_as_0130_98"},
  {109, "NNPDF30_lo_as_0130_99"},
  {110, "NNPDF30_lo_as_0130_100"},

  {111, "NNPDF30_lo_as_0130_nf4_0"},
  {112, "NNPDF30_lo_as_0130_nf4_1"},
  {113, "NNPDF30_lo_as_0130_nf4_2"},
  {114, "NNPDF30_lo_as_0130_nf4_3"},
  {115, "NNPDF30_lo_as_0130_nf4_4"},
  {116, "NNPDF30_lo_as_0130_nf4_5"},
  {117, "NNPDF30_lo_as_0130_nf4_6"},
  {118, "NNPDF30_lo_as_0130_nf4_7"},
  {119, "NNPDF30_lo_as_0130_nf4_8"},
  {120, "NNPDF30_lo_as_0130_nf4_9"},
  {121, "NNPDF30_lo_as_0130_nf4_10"},
  {122, "NNPDF30_lo_as_0130_nf4_11"},
  {123, "NNPDF30_lo_as_0130_nf4_12"},
  {124, "NNPDF30_lo_as_0130_nf4_13"},
  {125, "NNPDF30_lo_as_0130_nf4_14"},
  {126, "NNPDF30_lo_as_0130_nf4_15"},
  {127, "NNPDF30_lo_as_0130_nf4_16"},
  {128, "NNPDF30_lo_as_0130_nf4_17"},
  {129, "NNPDF30_lo_as_0130_nf4_18"},
  {130, "NNPDF30_lo_as_0130_nf4_19"},
  {131, "NNPDF30_lo_as_0130_nf4_20"},
  {132, "NNPDF30_lo_as_0130_nf4_21"},
  {133, "NNPDF30_lo_as_0130_nf4_22"},
  {134, "NNPDF30_lo_as_0130_nf4_23"},
  {135, "NNPDF30_lo_as_0130_nf4_24"},
  {136, "NNPDF30_lo_as_0130_nf4_25"},
  {137, "NNPDF30_lo_as_0130_nf4_26"},
  {138, "NNPDF30_lo_as_0130_nf4_27"},
  {139, "NNPDF30_lo_as_0130_nf4_28"},
  {140, "NNPDF30_lo_as_0130_nf4_29"},
  {141, "NNPDF30_lo_as_0130_nf4_30"},
  {142, "NNPDF30_lo_as_0130_nf4_31"},
  {143, "NNPDF30_lo_as_0130_nf4_32"},
  {144, "NNPDF30_lo_as_0130_nf4_33"},
  {145, "NNPDF30_lo_as_0130_nf4_34"},
  {146, "NNPDF30_lo_as_0130_nf4_35"},
  {147, "NNPDF30_lo_as_0130_nf4_36"},
  {148, "NNPDF30_lo_as_0130_nf4_37"},
  {149, "NNPDF30_lo_as_0130_nf4_38"},
  {150, "NNPDF30_lo_as_0130_nf4_39"},
  {151, "NNPDF30_lo_as_0130_nf4_40"},
  {152, "NNPDF30_lo_as_0130_nf4_41"},
  {153, "NNPDF30_lo_as_0130_nf4_42"},
  {154, "NNPDF30_lo_as_0130_nf4_43"},
  {155, "NNPDF30_lo_as_0130_nf4_44"},
  {156, "NNPDF30_lo_as_0130_nf4_45"},
  {157, "NNPDF30_lo_as_0130_nf4_46"},
  {158, "NNPDF30_lo_as_0130_nf4_47"},
  {159, "NNPDF30_lo_as_0130_nf4_48"},
  {160, "NNPDF30_lo_as_0130_nf4_49"},
  {161, "NNPDF30_lo_as_0130_nf4_50"},
  {162, "NNPDF30_lo_as_0130_nf4_51"},
  {163, "NNPDF30_lo_as_0130_nf4_52"},
  {164, "NNPDF30_lo_as_0130_nf4_53"},
  {165, "NNPDF30_lo_as_0130_nf4_54"},
  {166, "NNPDF30_lo_as_0130_nf4_55"},
  {167, "NNPDF30_lo_as_0130_nf4_56"},
  {168, "NNPDF30_lo_as_0130_nf4_57"},
  {169, "NNPDF30_lo_as_0130_nf4_58"},
  {170, "NNPDF30_lo_as_0130_nf4_59"},
  {171, "NNPDF30_lo_as_0130_nf4_60"},
  {172, "NNPDF30_lo_as_0130_nf4_61"},
  {173, "NNPDF30_lo_as_0130_nf4_62"},
  {174, "NNPDF30_lo_as_0130_nf4_63"},
  {175, "NNPDF30_lo_as_0130_nf4_64"},
  {176, "NNPDF30_lo_as_0130_nf4_65"},
  {177, "NNPDF30_lo_as_0130_nf4_66"},
  {178, "NNPDF30_lo_as_0130_nf4_67"},
  {179, "NNPDF30_lo_as_0130_nf4_68"},
  {180, "NNPDF30_lo_as_0130_nf4_69"},
  {181, "NNPDF30_lo_as_0130_nf4_70"},
  {182, "NNPDF30_lo_as_0130_nf4_71"},
  {183, "NNPDF30_lo_as_0130_nf4_72"},
  {184, "NNPDF30_lo_as_0130_nf4_73"},
  {185, "NNPDF30_lo_as_0130_nf4_74"},
  {186, "NNPDF30_lo_as_0130_nf4_75"},
  {187, "NNPDF30_lo_as_0130_nf4_76"},
  {188, "NNPDF30_lo_as_0130_nf4_77"},
  {189, "NNPDF30_lo_as_0130_nf4_78"},
  {190, "NNPDF30_lo_as_0130_nf4_79"},
  {191, "NNPDF30_lo_as_0130_nf4_80"},
  {192, "NNPDF30_lo_as_0130_nf4_81"},
  {193, "NNPDF30_lo_as_0130_nf4_82"},
  {194, "NNPDF30_lo_as_0130_nf4_83"},
  {195, "NNPDF30_lo_as_0130_nf4_84"},
  {196, "NNPDF30_lo_as_0130_nf4_85"},
  {197, "NNPDF30_lo_as_0130_nf4_86"},
  {198, "NNPDF30_lo_as_0130_nf4_87"},
  {199, "NNPDF30_lo_as_0130_nf4_88"},
  {200, "NNPDF30_lo_as_0130_nf4_89"},
  {201, "NNPDF30_lo_as_0130_nf4_90"},
  {202, "NNPDF30_lo_as_0130_nf4_91"},
  {203, "NNPDF30_lo_as_0130_nf4_92"},
  {204, "NNPDF30_lo_as_0130_nf4_93"},
  {205, "NNPDF30_lo_as_0130_nf4_94"},
  {206, "NNPDF30_lo_as_0130_nf4_95"},
  {207, "NNPDF30_lo_as_0130_nf4_96"},
  {208, "NNPDF30_lo_as_0130_nf4_97"},
  {209, "NNPDF30_lo_as_0130_nf4_98"},
  {210, "NNPDF30_lo_as_0130_nf4_99"},
  {211, "NNPDF30_lo_as_0130_nf4_100"},

  {212, "NNPDF30_lo_as_0118_0"},

  {213, "NNPDF23_lo_as_0130_qed_0"},
  {214, "NNPDF23_lo_as_0130_qed_1"},
  {215, "NNPDF23_lo_as_0130_qed_2"},
  {216, "NNPDF23_lo_as_0130_qed_3"},
  {217, "NNPDF23_lo_as_0130_qed_4"},
  {218, "NNPDF23_lo_as_0130_qed_5"},
  {219, "NNPDF23_lo_as_0130_qed_6"},
  {220, "NNPDF23_lo_as_0130_qed_7"},
  {221, "NNPDF23_lo_as_0130_qed_8"},
  {222, "NNPDF23_lo_as_0130_qed_9"},
  {223, "NNPDF23_lo_as_0130_qed_10"},
  {224, "NNPDF23_lo_as_0130_qed_11"},
  {225, "NNPDF23_lo_as_0130_qed_12"},
  {226, "NNPDF23_lo_as_0130_qed_13"},
  {227, "NNPDF23_lo_as_0130_qed_14"},
  {228, "NNPDF23_lo_as_0130_qed_15"},
  {229, "NNPDF23_lo_as_0130_qed_16"},
  {230, "NNPDF23_lo_as_0130_qed_17"},
  {231, "NNPDF23_lo_as_0130_qed_18"},
  {232, "NNPDF23_lo_as_0130_qed_19"},
  {233, "NNPDF23_lo_as_0130_qed_20"},
  {234, "NNPDF23_lo_as_0130_qed_21"},
  {235, "NNPDF23_lo_as_0130_qed_22"},
  {236, "NNPDF23_lo_as_0130_qed_23"},
  {237, "NNPDF23_lo_as_0130_qed_24"},
  {238, "NNPDF23_lo_as_0130_qed_25"},
  {239, "NNPDF23_lo_as_0130_qed_26"},
  {240, "NNPDF23_lo_as_0130_qed_27"},
  {241, "NNPDF23_lo_as_0130_qed_28"},
  {242, "NNPDF23_lo_as_0130_qed_29"},
  {243, "NNPDF23_lo_as_0130_qed_30"},
  {244, "NNPDF23_lo_as_0130_qed_31"},
  {245, "NNPDF23_lo_as_0130_qed_32"},
  {246, "NNPDF23_lo_as_0130_qed_33"},
  {247, "NNPDF23_lo_as_0130_qed_34"},
  {248, "NNPDF23_lo_as_0130_qed_35"},
  {249, "NNPDF23_lo_as_0130_qed_36"},
  {250, "NNPDF23_lo_as_0130_qed_37"},
  {251, "NNPDF23_lo_as_0130_qed_38"},
  {252, "NNPDF23_lo_as_0130_qed_39"},
  {253, "NNPDF23_lo_as_0130_qed_40"},
  {254, "NNPDF23_lo_as_0130_qed_41"},
  {255, "NNPDF23_lo_as_0130_qed_42"},
  {256, "NNPDF23_lo_as_0130_qed_43"},
  {257, "NNPDF23_lo_as_0130_qed_44"},
  {258, "NNPDF23_lo_as_0130_qed_45"},
  {259, "NNPDF23_lo_as_0130_qed_46"},
  {260, "NNPDF23_lo_as_0130_qed_47"},
  {261, "NNPDF23_lo_as_0130_qed_48"},
  {262, "NNPDF23_lo_as_0130_qed_49"},
  {263, "NNPDF23_lo_as_0130_qed_50"},
  {264, "NNPDF23_lo_as_0130_qed_51"},
  {265, "NNPDF23_lo_as_0130_qed_52"},
  {266, "NNPDF23_lo_as_0130_qed_53"},
  {267, "NNPDF23_lo_as_0130_qed_54"},
  {268, "NNPDF23_lo_as_0130_qed_55"},
  {269, "NNPDF23_lo_as_0130_qed_56"},
  {270, "NNPDF23_lo_as_0130_qed_57"},
  {271, "NNPDF23_lo_as_0130_qed_58"},
  {272, "NNPDF23_lo_as_0130_qed_59"},
  {273, "NNPDF23_lo_as_0130_qed_60"},
  {274, "NNPDF23_lo_as_0130_qed_61"},
  {275, "NNPDF23_lo_as_0130_qed_62"},
  {276, "NNPDF23_lo_as_0130_qed_63"},
  {277, "NNPDF23_lo_as_0130_qed_64"},
  {278, "NNPDF23_lo_as_0130_qed_65"},
  {279, "NNPDF23_lo_as_0130_qed_66"},
  {280, "NNPDF23_lo_as_0130_qed_67"},
  {281, "NNPDF23_lo_as_0130_qed_68"},
  {282, "NNPDF23_lo_as_0130_qed_69"},
  {283, "NNPDF23_lo_as_0130_qed_70"},
  {284, "NNPDF23_lo_as_0130_qed_71"},
  {285, "NNPDF23_lo_as_0130_qed_72"},
  {286, "NNPDF23_lo_as_0130_qed_73"},
  {287, "NNPDF23_lo_as_0130_qed_74"},
  {288, "NNPDF23_lo_as_0130_qed_75"},
  {289, "NNPDF23_lo_as_0130_qed_76"},
  {290, "NNPDF23_lo_as_0130_qed_77"},
  {291, "NNPDF23_lo_as_0130_qed_78"},
  {292, "NNPDF23_lo_as_0130_qed_79"},
  {293, "NNPDF23_lo_as_0130_qed_80"},
  {294, "NNPDF23_lo_as_0130_qed_81"},
  {295, "NNPDF23_lo_as_0130_qed_82"},
  {296, "NNPDF23_lo_as_0130_qed_83"},
  {297, "NNPDF23_lo_as_0130_qed_84"},
  {298, "NNPDF23_lo_as_0130_qed_85"},
  {299, "NNPDF23_lo_as_0130_qed_86"},
  {300, "NNPDF23_lo_as_0130_qed_87"},
  {301, "NNPDF23_lo_as_0130_qed_88"},
  {302, "NNPDF23_lo_as_0130_qed_89"},
  {303, "NNPDF23_lo_as_0130_qed_90"},
  {304, "NNPDF23_lo_as_0130_qed_91"},
  {305, "NNPDF23_lo_as_0130_qed_92"},
  {306, "NNPDF23_lo_as_0130_qed_93"},
  {307, "NNPDF23_lo_as_0130_qed_94"},
  {308, "NNPDF23_lo_as_0130_qed_95"},
  {309, "NNPDF23_lo_as_0130_qed_96"},
  {310, "NNPDF23_lo_as_0130_qed_97"},
  {311, "NNPDF23_lo_as_0130_qed_98"},
  {312, "NNPDF23_lo_as_0130_qed_99"},
  {313, "NNPDF23_lo_as_0130_qed_100"},

  {314, "NNPDF23_lo_as_0119_qed_0"},

  {315, "CTEQ6l1_0"},

  {316, "MMHT2014lo68cl_0"},
  {317, "MMHT2014lo68cl_1"},
  {318, "MMHT2014lo68cl_2"},
  {319, "MMHT2014lo68cl_3"},
  {320, "MMHT2014lo68cl_4"},
  {321, "MMHT2014lo68cl_5"},
  {322, "MMHT2014lo68cl_6"},
  {323, "MMHT2014lo68cl_7"},
  {324, "MMHT2014lo68cl_8"},
  {325, "MMHT2014lo68cl_9"},
  {326, "MMHT2014lo68cl_10"},
  {327, "MMHT2014lo68cl_11"},
  {328, "MMHT2014lo68cl_12"},
  {329, "MMHT2014lo68cl_13"},
  {330, "MMHT2014lo68cl_14"},
  {331, "MMHT2014lo68cl_15"},
  {332, "MMHT2014lo68cl_16"},
  {333, "MMHT2014lo68cl_17"},
  {334, "MMHT2014lo68cl_18"},
  {335, "MMHT2014lo68cl_19"},
  {336, "MMHT2014lo68cl_20"},
  {337, "MMHT2014lo68cl_21"},
  {338, "MMHT2014lo68cl_22"},
  {339, "MMHT2014lo68cl_23"},
  {340, "MMHT2014lo68cl_24"},
  {341, "MMHT2014lo68cl_25"},
  {342, "MMHT2014lo68cl_26"},
  {343, "MMHT2014lo68cl_27"},
  {344, "MMHT2014lo68cl_28"},
  {345, "MMHT2014lo68cl_29"},
  {346, "MMHT2014lo68cl_30"},
  {347, "MMHT2014lo68cl_31"},
  {348, "MMHT2014lo68cl_32"},
  {349, "MMHT2014lo68cl_33"},
  {350, "MMHT2014lo68cl_34"},
  {351, "MMHT2014lo68cl_35"},
  {352, "MMHT2014lo68cl_36"},
  {353, "MMHT2014lo68cl_37"},
  {354, "MMHT2014lo68cl_38"},
  {355, "MMHT2014lo68cl_39"},
  {356, "MMHT2014lo68cl_40"},
  {357, "MMHT2014lo68cl_41"},
  {358, "MMHT2014lo68cl_42"},
  {359, "MMHT2014lo68cl_43"},
  {360, "MMHT2014lo68cl_44"},
  {361, "MMHT2014lo68cl_45"},
  {362, "MMHT2014lo68cl_46"},
  {363, "MMHT2014lo68cl_47"},
  {364, "MMHT2014lo68cl_48"},
  {365, "MMHT2014lo68cl_49"},
  {366, "MMHT2014lo68cl_50"},

  {367, "MMHT2014lo_asmzsmallrange_0"},
  {368, "MMHT2014lo_asmzsmallrange_1"},
  {369, "MMHT2014lo_asmzsmallrange_2"},

  {370, "HERAPDF15LO_EIG_0"},
  {371, "HERAPDF15LO_EIG_1"},
  {372, "HERAPDF15LO_EIG_2"},
  {373, "HERAPDF15LO_EIG_3"},
  {374, "HERAPDF15LO_EIG_4"},
  {375, "HERAPDF15LO_EIG_5"},
  {376, "HERAPDF15LO_EIG_6"},
  {377, "HERAPDF15LO_EIG_7"},
  {378, "HERAPDF15LO_EIG_8"},
  {379, "HERAPDF15LO_EIG_9"},
  {380, "HERAPDF15LO_EIG_10"},
  {381, "HERAPDF15LO_EIG_11"},
  {382, "HERAPDF15LO_EIG_12"},
  {383, "HERAPDF15LO_EIG_13"},
  {384, "HERAPDF15LO_EIG_14"},
  {385, "HERAPDF15LO_EIG_15"},
  {386, "HERAPDF15LO_EIG_16"},
  {387, "HERAPDF15LO_EIG_17"},
  {388, "HERAPDF15LO_EIG_18"},
  {389, "HERAPDF15LO_EIG_19"},
  {390, "HERAPDF15LO_EIG_20"},

  {391, "NNPDF30_nlo_as_0118_0"},

  {392, "NNPDF23_nlo_as_0119_0"},

  {393, "CT10nlo_0"},
  {394, "CT10nlo_1"},
  {395, "CT10nlo_2"},
  {396, "CT10nlo_3"},
  {397, "CT10nlo_4"},
  {398, "CT10nlo_5"},
  {399, "CT10nlo_6"},
  {400, "CT10nlo_7"},
  {401, "CT10nlo_8"},
  {402, "CT10nlo_9"},
  {403, "CT10nlo_10"},
  {404, "CT10nlo_11"},
  {405, "CT10nlo_12"},
  {406, "CT10nlo_13"},
  {407, "CT10nlo_14"},
  {408, "CT10nlo_15"},
  {409, "CT10nlo_16"},
  {410, "CT10nlo_17"},
  {411, "CT10nlo_18"},
  {412, "CT10nlo_19"},
  {413, "CT10nlo_20"},
  {414, "CT10nlo_21"},
  {415, "CT10nlo_22"},
  {416, "CT10nlo_23"},
  {417, "CT10nlo_24"},
  {418, "CT10nlo_25"},
  {419, "CT10nlo_26"},
  {420, "CT10nlo_27"},
  {421, "CT10nlo_28"},
  {422, "CT10nlo_29"},
  {423, "CT10nlo_30"},
  {424, "CT10nlo_31"},
  {425, "CT10nlo_32"},
  {426, "CT10nlo_33"},
  {427, "CT10nlo_34"},
  {428, "CT10nlo_35"},
  {429, "CT10nlo_36"},
  {430, "CT10nlo_37"},
  {431, "CT10nlo_38"},
  {432, "CT10nlo_39"},
  {433, "CT10nlo_40"},
  {434, "CT10nlo_41"},
  {435, "CT10nlo_42"},
  {436, "CT10nlo_43"},
  {437, "CT10nlo_44"},
  {438, "CT10nlo_45"},
  {439, "CT10nlo_46"},
  {440, "CT10nlo_47"},
  {441, "CT10nlo_48"},
  {442, "CT10nlo_49"},
  {443, "CT10nlo_50"},
  {444, "CT10nlo_51"},
  {445, "CT10nlo_52"},

  {446, "MMHT2014nlo68cl_0"}
};

std::map<int,std::string> LHEweightMAP254 = {
  {1001, "Q2_0"},
  {1002, "Q2_1"},
  {1003, "Q2_2"},
  {1004, "Q2_3"},
  {1005, "Q2_4"},
  {1006, "Q2_5"},
  {1007, "Q2_6"},
  {1008, "Q2_7"},
  {1009, "Q2_8"},

  {2001, "NNPDF30_nlo_as_0118_1"},
  {2002, "NNPDF30_nlo_as_0118_2"},
  {2003, "NNPDF30_nlo_as_0118_3"},
  {2004, "NNPDF30_nlo_as_0118_4"},
  {2005, "NNPDF30_nlo_as_0118_5"},
  {2006, "NNPDF30_nlo_as_0118_6"},
  {2007, "NNPDF30_nlo_as_0118_7"},
  {2008, "NNPDF30_nlo_as_0118_8"},
  {2009, "NNPDF30_nlo_as_0118_9"},
  {2010, "NNPDF30_nlo_as_0118_10"},
  {2011, "NNPDF30_nlo_as_0118_11"},
  {2012, "NNPDF30_nlo_as_0118_12"},
  {2013, "NNPDF30_nlo_as_0118_13"},
  {2014, "NNPDF30_nlo_as_0118_14"},
  {2015, "NNPDF30_nlo_as_0118_15"},
  {2016, "NNPDF30_nlo_as_0118_16"},
  {2017, "NNPDF30_nlo_as_0118_17"},
  {2018, "NNPDF30_nlo_as_0118_18"},
  {2019, "NNPDF30_nlo_as_0118_19"},
  {2020, "NNPDF30_nlo_as_0118_20"},
  {2021, "NNPDF30_nlo_as_0118_21"},
  {2022, "NNPDF30_nlo_as_0118_22"},
  {2023, "NNPDF30_nlo_as_0118_23"},
  {2024, "NNPDF30_nlo_as_0118_24"},
  {2025, "NNPDF30_nlo_as_0118_25"},
  {2026, "NNPDF30_nlo_as_0118_26"},
  {2027, "NNPDF30_nlo_as_0118_27"},
  {2028, "NNPDF30_nlo_as_0118_28"},
  {2029, "NNPDF30_nlo_as_0118_29"},
  {2030, "NNPDF30_nlo_as_0118_30"},
  {2031, "NNPDF30_nlo_as_0118_31"},
  {2032, "NNPDF30_nlo_as_0118_32"},
  {2033, "NNPDF30_nlo_as_0118_33"},
  {2034, "NNPDF30_nlo_as_0118_34"},
  {2035, "NNPDF30_nlo_as_0118_35"},
  {2036, "NNPDF30_nlo_as_0118_36"},
  {2037, "NNPDF30_nlo_as_0118_37"},
  {2038, "NNPDF30_nlo_as_0118_38"},
  {2039, "NNPDF30_nlo_as_0118_39"},
  {2040, "NNPDF30_nlo_as_0118_40"},
  {2041, "NNPDF30_nlo_as_0118_41"},
  {2042, "NNPDF30_nlo_as_0118_42"},
  {2043, "NNPDF30_nlo_as_0118_43"},
  {2044, "NNPDF30_nlo_as_0118_44"},
  {2045, "NNPDF30_nlo_as_0118_45"},
  {2046, "NNPDF30_nlo_as_0118_46"},
  {2047, "NNPDF30_nlo_as_0118_47"},
  {2048, "NNPDF30_nlo_as_0118_48"},
  {2049, "NNPDF30_nlo_as_0118_49"},
  {2050, "NNPDF30_nlo_as_0118_50"},
  {2051, "NNPDF30_nlo_as_0118_51"},
  {2052, "NNPDF30_nlo_as_0118_52"},
  {2053, "NNPDF30_nlo_as_0118_53"},
  {2054, "NNPDF30_nlo_as_0118_54"},
  {2055, "NNPDF30_nlo_as_0118_55"},
  {2056, "NNPDF30_nlo_as_0118_56"},
  {2057, "NNPDF30_nlo_as_0118_57"},
  {2058, "NNPDF30_nlo_as_0118_58"},
  {2059, "NNPDF30_nlo_as_0118_59"},
  {2060, "NNPDF30_nlo_as_0118_60"},
  {2061, "NNPDF30_nlo_as_0118_61"},
  {2062, "NNPDF30_nlo_as_0118_62"},
  {2063, "NNPDF30_nlo_as_0118_63"},
  {2064, "NNPDF30_nlo_as_0118_64"},
  {2065, "NNPDF30_nlo_as_0118_65"},
  {2066, "NNPDF30_nlo_as_0118_66"},
  {2067, "NNPDF30_nlo_as_0118_67"},
  {2068, "NNPDF30_nlo_as_0118_68"},
  {2069, "NNPDF30_nlo_as_0118_69"},
  {2070, "NNPDF30_nlo_as_0118_70"},
  {2071, "NNPDF30_nlo_as_0118_71"},
  {2072, "NNPDF30_nlo_as_0118_72"},
  {2073, "NNPDF30_nlo_as_0118_73"},
  {2074, "NNPDF30_nlo_as_0118_74"},
  {2075, "NNPDF30_nlo_as_0118_75"},
  {2076, "NNPDF30_nlo_as_0118_76"},
  {2077, "NNPDF30_nlo_as_0118_77"},
  {2078, "NNPDF30_nlo_as_0118_78"},
  {2079, "NNPDF30_nlo_as_0118_79"},
  {2080, "NNPDF30_nlo_as_0118_80"},
  {2081, "NNPDF30_nlo_as_0118_81"},
  {2082, "NNPDF30_nlo_as_0118_82"},
  {2083, "NNPDF30_nlo_as_0118_83"},
  {2084, "NNPDF30_nlo_as_0118_84"},
  {2085, "NNPDF30_nlo_as_0118_85"},
  {2086, "NNPDF30_nlo_as_0118_86"},
  {2087, "NNPDF30_nlo_as_0118_87"},
  {2088, "NNPDF30_nlo_as_0118_88"},
  {2089, "NNPDF30_nlo_as_0118_89"},
  {2090, "NNPDF30_nlo_as_0118_90"},
  {2091, "NNPDF30_nlo_as_0118_91"},
  {2092, "NNPDF30_nlo_as_0118_92"},
  {2093, "NNPDF30_nlo_as_0118_93"},
  {2094, "NNPDF30_nlo_as_0118_94"},
  {2095, "NNPDF30_nlo_as_0118_95"},
  {2096, "NNPDF30_nlo_as_0118_96"},
  {2097, "NNPDF30_nlo_as_0118_97"},
  {2098, "NNPDF30_nlo_as_0118_98"},
  {2099, "NNPDF30_nlo_as_0118_99"},
  {2100, "NNPDF30_nlo_as_0118_100"},

  {2101, "NNPDF30_nlo_as_0117_0"},

  {2102, "NNPDF30_nlo_as_0119_0"},

  {3001, "CT14nlo_0"},
  {3002, "CT14nlo_1"},
  {3003, "CT14nlo_2"},
  {3004, "CT14nlo_3"},
  {3005, "CT14nlo_4"},
  {3006, "CT14nlo_5"},
  {3007, "CT14nlo_6"},
  {3008, "CT14nlo_7"},
  {3009, "CT14nlo_8"},
  {3010, "CT14nlo_9"},
  {3011, "CT14nlo_10"},
  {3012, "CT14nlo_11"},
  {3013, "CT14nlo_12"},
  {3014, "CT14nlo_13"},
  {3015, "CT14nlo_14"},
  {3016, "CT14nlo_15"},
  {3017, "CT14nlo_16"},
  {3018, "CT14nlo_17"},
  {3019, "CT14nlo_18"},
  {3020, "CT14nlo_19"},
  {3021, "CT14nlo_20"},
  {3022, "CT14nlo_21"},
  {3023, "CT14nlo_22"},
  {3024, "CT14nlo_23"},
  {3025, "CT14nlo_24"},
  {3026, "CT14nlo_25"},
  {3027, "CT14nlo_26"},
  {3028, "CT14nlo_27"},
  {3029, "CT14nlo_28"},
  {3030, "CT14nlo_29"},
  {3031, "CT14nlo_30"},
  {3032, "CT14nlo_31"},
  {3033, "CT14nlo_32"},
  {3034, "CT14nlo_33"},
  {3035, "CT14nlo_34"},
  {3036, "CT14nlo_35"},
  {3037, "CT14nlo_36"},
  {3038, "CT14nlo_37"},
  {3039, "CT14nlo_38"},
  {3040, "CT14nlo_39"},
  {3041, "CT14nlo_40"},
  {3042, "CT14nlo_41"},
  {3043, "CT14nlo_42"},
  {3044, "CT14nlo_43"},
  {3045, "CT14nlo_44"},
  {3046, "CT14nlo_45"},
  {3047, "CT14nlo_46"},
  {3048, "CT14nlo_47"},
  {3049, "CT14nlo_48"},
  {3050, "CT14nlo_49"},
  {3051, "CT14nlo_50"},
  {3052, "CT14nlo_51"},
  {3053, "CT14nlo_52"},
  {3054, "CT14nlo_53"},
  {3055, "CT14nlo_54"},
  {3056, "CT14nlo_55"},
  {3057, "CT14nlo_56"},

  {3058, "CT14nlo_as_0117"},

  {3059, "CT14nlo_as_0119"},

  {3060, "CT10wnlo_0"},

  {4001, "MMHT2014nlo68clas118_0"},
  {4002, "MMHT2014nlo68clas118_1"},
  {4003, "MMHT2014nlo68clas118_2"},
  {4004, "MMHT2014nlo68clas118_3"},
  {4005, "MMHT2014nlo68clas118_4"},
  {4006, "MMHT2014nlo68clas118_5"},
  {4007, "MMHT2014nlo68clas118_6"},
  {4008, "MMHT2014nlo68clas118_7"},
  {4009, "MMHT2014nlo68clas118_8"},
  {4010, "MMHT2014nlo68clas118_9"},
  {4011, "MMHT2014nlo68clas118_10"},
  {4012, "MMHT2014nlo68clas118_11"},
  {4013, "MMHT2014nlo68clas118_12"},
  {4014, "MMHT2014nlo68clas118_13"},
  {4015, "MMHT2014nlo68clas118_14"},
  {4016, "MMHT2014nlo68clas118_15"},
  {4017, "MMHT2014nlo68clas118_16"},
  {4018, "MMHT2014nlo68clas118_17"},
  {4019, "MMHT2014nlo68clas118_18"},
  {4020, "MMHT2014nlo68clas118_19"},
  {4021, "MMHT2014nlo68clas118_20"},
  {4022, "MMHT2014nlo68clas118_21"},
  {4023, "MMHT2014nlo68clas118_22"},
  {4024, "MMHT2014nlo68clas118_23"},
  {4025, "MMHT2014nlo68clas118_24"},
  {4026, "MMHT2014nlo68clas118_25"},
  {4027, "MMHT2014nlo68clas118_26"},
  {4028, "MMHT2014nlo68clas118_27"},
  {4029, "MMHT2014nlo68clas118_28"},
  {4030, "MMHT2014nlo68clas118_29"},
  {4031, "MMHT2014nlo68clas118_30"},
  {4032, "MMHT2014nlo68clas118_31"},
  {4033, "MMHT2014nlo68clas118_32"},
  {4034, "MMHT2014nlo68clas118_33"},
  {4035, "MMHT2014nlo68clas118_34"},
  {4036, "MMHT2014nlo68clas118_35"},
  {4037, "MMHT2014nlo68clas118_36"},
  {4038, "MMHT2014nlo68clas118_37"},
  {4039, "MMHT2014nlo68clas118_38"},
  {4040, "MMHT2014nlo68clas118_39"},
  {4041, "MMHT2014nlo68clas118_40"},
  {4042, "MMHT2014nlo68clas118_41"},
  {4043, "MMHT2014nlo68clas118_42"},
  {4044, "MMHT2014nlo68clas118_43"},
  {4045, "MMHT2014nlo68clas118_44"},
  {4046, "MMHT2014nlo68clas118_45"},
  {4047, "MMHT2014nlo68clas118_46"},
  {4048, "MMHT2014nlo68clas118_47"},
  {4049, "MMHT2014nlo68clas118_48"},
  {4050, "MMHT2014nlo68clas118_49"},
  {4051, "MMHT2014nlo68clas118_50"},

  {4052, "MMHT2014nlo_asmzsmallrange_0"},
  {4053, "MMHT2014nlo_asmzsmallrange_1"},
  {4054, "MMHT2014nlo_asmzsmallrange_2"},
  {4055, "MMHT2014nlo_asmzsmallrange_3"},
  {4056, "MMHT2014nlo_asmzsmallrange_4"},

  {5001, "hdamp_0"},
  {5002, "hdamp_1"},
  {5003, "hdamp_2"},
  {5004, "hdamp_3"},
  {5005, "hdamp_4"},
  {5006, "hdamp_5"},
  {5007, "hdamp_6"},
  {5008, "hdamp_7"},
  {5009, "hdamp_8"},
  {5010, "hdamp_9"},
  {5011, "hdamp_10"},
  {5012, "hdamp_11"},
  {5013, "hdamp_12"},
  {5014, "hdamp_13"},
  {5015, "hdamp_14"},
  {5016, "hdamp_15"},
  {5017, "hdamp_16"},
  {5018, "hdamp_17"},
  {5019, "hdamp_18"},
  {5020, "hdamp_19"},
  {5021, "hdamp_20"},
  {5022, "hdamp_21"},
  {5023, "hdamp_22"},
  {5024, "hdamp_23"},
  {5025, "hdamp_24"},
  {5026, "hdamp_25"},
  {5027, "hdamp_26"}
};

std::map<int,std::string> LHEweightMAP111 = {
  {1001, "Q2_0"},
  {1002, "Q2_1"},
  {1003, "Q2_2"},
  {1004, "Q2_3"},
  {1005, "Q2_4"},
  {1006, "Q2_5"},
  {1007, "Q2_6"},
  {1008, "Q2_7"},
  {1009, "Q2_8"},

  {2001, "NNPDF30_nlo_nf_5_pdfas_1"},
  {2002, "NNPDF30_nlo_nf_5_pdfas_2"},
  {2003, "NNPDF30_nlo_nf_5_pdfas_3"},
  {2004, "NNPDF30_nlo_nf_5_pdfas_4"},
  {2005, "NNPDF30_nlo_nf_5_pdfas_5"},
  {2006, "NNPDF30_nlo_nf_5_pdfas_6"},
  {2007, "NNPDF30_nlo_nf_5_pdfas_7"},
  {2008, "NNPDF30_nlo_nf_5_pdfas_8"},
  {2009, "NNPDF30_nlo_nf_5_pdfas_9"},
  {2010, "NNPDF30_nlo_nf_5_pdfas_10"},
  {2011, "NNPDF30_nlo_nf_5_pdfas_11"},
  {2012, "NNPDF30_nlo_nf_5_pdfas_12"},
  {2013, "NNPDF30_nlo_nf_5_pdfas_13"},
  {2014, "NNPDF30_nlo_nf_5_pdfas_14"},
  {2015, "NNPDF30_nlo_nf_5_pdfas_15"},
  {2016, "NNPDF30_nlo_nf_5_pdfas_16"},
  {2017, "NNPDF30_nlo_nf_5_pdfas_17"},
  {2018, "NNPDF30_nlo_nf_5_pdfas_18"},
  {2019, "NNPDF30_nlo_nf_5_pdfas_19"},
  {2020, "NNPDF30_nlo_nf_5_pdfas_20"},
  {2021, "NNPDF30_nlo_nf_5_pdfas_21"},
  {2022, "NNPDF30_nlo_nf_5_pdfas_22"},
  {2023, "NNPDF30_nlo_nf_5_pdfas_23"},
  {2024, "NNPDF30_nlo_nf_5_pdfas_24"},
  {2025, "NNPDF30_nlo_nf_5_pdfas_25"},
  {2026, "NNPDF30_nlo_nf_5_pdfas_26"},
  {2027, "NNPDF30_nlo_nf_5_pdfas_27"},
  {2028, "NNPDF30_nlo_nf_5_pdfas_28"},
  {2029, "NNPDF30_nlo_nf_5_pdfas_29"},
  {2030, "NNPDF30_nlo_nf_5_pdfas_30"},
  {2031, "NNPDF30_nlo_nf_5_pdfas_31"},
  {2032, "NNPDF30_nlo_nf_5_pdfas_32"},
  {2033, "NNPDF30_nlo_nf_5_pdfas_33"},
  {2034, "NNPDF30_nlo_nf_5_pdfas_34"},
  {2035, "NNPDF30_nlo_nf_5_pdfas_35"},
  {2036, "NNPDF30_nlo_nf_5_pdfas_36"},
  {2037, "NNPDF30_nlo_nf_5_pdfas_37"},
  {2038, "NNPDF30_nlo_nf_5_pdfas_38"},
  {2039, "NNPDF30_nlo_nf_5_pdfas_39"},
  {2040, "NNPDF30_nlo_nf_5_pdfas_40"},
  {2041, "NNPDF30_nlo_nf_5_pdfas_41"},
  {2042, "NNPDF30_nlo_nf_5_pdfas_42"},
  {2043, "NNPDF30_nlo_nf_5_pdfas_43"},
  {2044, "NNPDF30_nlo_nf_5_pdfas_44"},
  {2045, "NNPDF30_nlo_nf_5_pdfas_45"},
  {2046, "NNPDF30_nlo_nf_5_pdfas_46"},
  {2047, "NNPDF30_nlo_nf_5_pdfas_47"},
  {2048, "NNPDF30_nlo_nf_5_pdfas_48"},
  {2049, "NNPDF30_nlo_nf_5_pdfas_49"},
  {2050, "NNPDF30_nlo_nf_5_pdfas_50"},
  {2051, "NNPDF30_nlo_nf_5_pdfas_51"},
  {2052, "NNPDF30_nlo_nf_5_pdfas_52"},
  {2053, "NNPDF30_nlo_nf_5_pdfas_53"},
  {2054, "NNPDF30_nlo_nf_5_pdfas_54"},
  {2055, "NNPDF30_nlo_nf_5_pdfas_55"},
  {2056, "NNPDF30_nlo_nf_5_pdfas_56"},
  {2057, "NNPDF30_nlo_nf_5_pdfas_57"},
  {2058, "NNPDF30_nlo_nf_5_pdfas_58"},
  {2059, "NNPDF30_nlo_nf_5_pdfas_59"},
  {2060, "NNPDF30_nlo_nf_5_pdfas_60"},
  {2061, "NNPDF30_nlo_nf_5_pdfas_61"},
  {2062, "NNPDF30_nlo_nf_5_pdfas_62"},
  {2063, "NNPDF30_nlo_nf_5_pdfas_63"},
  {2064, "NNPDF30_nlo_nf_5_pdfas_64"},
  {2065, "NNPDF30_nlo_nf_5_pdfas_65"},
  {2066, "NNPDF30_nlo_nf_5_pdfas_66"},
  {2067, "NNPDF30_nlo_nf_5_pdfas_67"},
  {2068, "NNPDF30_nlo_nf_5_pdfas_68"},
  {2069, "NNPDF30_nlo_nf_5_pdfas_69"},
  {2070, "NNPDF30_nlo_nf_5_pdfas_70"},
  {2071, "NNPDF30_nlo_nf_5_pdfas_71"},
  {2072, "NNPDF30_nlo_nf_5_pdfas_72"},
  {2073, "NNPDF30_nlo_nf_5_pdfas_73"},
  {2074, "NNPDF30_nlo_nf_5_pdfas_74"},
  {2075, "NNPDF30_nlo_nf_5_pdfas_75"},
  {2076, "NNPDF30_nlo_nf_5_pdfas_76"},
  {2077, "NNPDF30_nlo_nf_5_pdfas_77"},
  {2078, "NNPDF30_nlo_nf_5_pdfas_78"},
  {2079, "NNPDF30_nlo_nf_5_pdfas_79"},
  {2080, "NNPDF30_nlo_nf_5_pdfas_80"},
  {2081, "NNPDF30_nlo_nf_5_pdfas_81"},
  {2082, "NNPDF30_nlo_nf_5_pdfas_82"},
  {2083, "NNPDF30_nlo_nf_5_pdfas_83"},
  {2084, "NNPDF30_nlo_nf_5_pdfas_84"},
  {2085, "NNPDF30_nlo_nf_5_pdfas_85"},
  {2086, "NNPDF30_nlo_nf_5_pdfas_86"},
  {2087, "NNPDF30_nlo_nf_5_pdfas_87"},
  {2088, "NNPDF30_nlo_nf_5_pdfas_88"},
  {2089, "NNPDF30_nlo_nf_5_pdfas_89"},
  {2090, "NNPDF30_nlo_nf_5_pdfas_90"},
  {2091, "NNPDF30_nlo_nf_5_pdfas_91"},
  {2092, "NNPDF30_nlo_nf_5_pdfas_92"},
  {2093, "NNPDF30_nlo_nf_5_pdfas_93"},
  {2094, "NNPDF30_nlo_nf_5_pdfas_94"},
  {2095, "NNPDF30_nlo_nf_5_pdfas_95"},
  {2096, "NNPDF30_nlo_nf_5_pdfas_96"},
  {2097, "NNPDF30_nlo_nf_5_pdfas_97"},
  {2098, "NNPDF30_nlo_nf_5_pdfas_98"},
  {2099, "NNPDF30_nlo_nf_5_pdfas_99"},
  {2100, "NNPDF30_nlo_nf_5_pdfas_100"},
  {2101, "NNPDF30_nlo_nf_5_pdfas_101"},
  {2102, "NNPDF30_nlo_nf_5_pdfas_102"},
  {2103, "NNPDF30_nlo_nf_5_pdfas_103"},
  {2104, "NNPDF30_nlo_nf_5_pdfas_104"},
  {2105, "NNPDF30_nlo_nf_5_pdfas_105"},
  {2106, "NNPDF30_nlo_nf_5_pdfas_106"},
  {2107, "NNPDF30_nlo_nf_5_pdfas_107"},
  {2108, "NNPDF30_nlo_nf_5_pdfas_108"},
  {2109, "NNPDF30_nlo_nf_5_pdfas_109"},
  {2110, "NNPDF30_nlo_nf_5_pdfas_110"},
  {2111, "NNPDF30_nlo_nf_5_pdfas_111"},
  {2112, "NNPDF30_nlo_nf_5_pdfas_112"},
  {2113, "NNPDF30_nlo_nf_5_pdfas_113"},
  {2114, "NNPDF30_nlo_nf_5_pdfas_114"},
  {2115, "NNPDF30_nlo_nf_5_pdfas_115"},
  {2116, "NNPDF30_nlo_nf_5_pdfas_116"},
  {2117, "NNPDF30_nlo_nf_5_pdfas_117"},
  {2118, "NNPDF30_nlo_nf_5_pdfas_118"},
  {2119, "NNPDF30_nlo_nf_5_pdfas_119"},
};

std::map<int,std::string> LHEweightMAP266 = {
  {1001, "Q2_0"},
  {1002, "Q2_1"},
  {1003, "Q2_2"},
  {1004, "Q2_3"},
  {1005, "Q2_4"},
  {1006, "Q2_5"},
  {1007, "Q2_6"},
  {1008, "Q2_7"},
  {1009, "Q2_8"},

  {2001, "NNPDF30_nlo_nf_4_pdfas_1"},
  {2002, "NNPDF30_nlo_nf_4_pdfas_2"},
  {2003, "NNPDF30_nlo_nf_4_pdfas_3"},
  {2004, "NNPDF30_nlo_nf_4_pdfas_4"},
  {2005, "NNPDF30_nlo_nf_4_pdfas_5"},
  {2006, "NNPDF30_nlo_nf_4_pdfas_6"},
  {2007, "NNPDF30_nlo_nf_4_pdfas_7"},
  {2008, "NNPDF30_nlo_nf_4_pdfas_8"},
  {2009, "NNPDF30_nlo_nf_4_pdfas_9"},
  {2010, "NNPDF30_nlo_nf_4_pdfas_10"},
  {2011, "NNPDF30_nlo_nf_4_pdfas_11"},
  {2012, "NNPDF30_nlo_nf_4_pdfas_12"},
  {2013, "NNPDF30_nlo_nf_4_pdfas_13"},
  {2014, "NNPDF30_nlo_nf_4_pdfas_14"},
  {2015, "NNPDF30_nlo_nf_4_pdfas_15"},
  {2016, "NNPDF30_nlo_nf_4_pdfas_16"},
  {2017, "NNPDF30_nlo_nf_4_pdfas_17"},
  {2018, "NNPDF30_nlo_nf_4_pdfas_18"},
  {2019, "NNPDF30_nlo_nf_4_pdfas_19"},
  {2020, "NNPDF30_nlo_nf_4_pdfas_20"},
  {2021, "NNPDF30_nlo_nf_4_pdfas_21"},
  {2022, "NNPDF30_nlo_nf_4_pdfas_22"},
  {2023, "NNPDF30_nlo_nf_4_pdfas_23"},
  {2024, "NNPDF30_nlo_nf_4_pdfas_24"},
  {2025, "NNPDF30_nlo_nf_4_pdfas_25"},
  {2026, "NNPDF30_nlo_nf_4_pdfas_26"},
  {2027, "NNPDF30_nlo_nf_4_pdfas_27"},
  {2028, "NNPDF30_nlo_nf_4_pdfas_28"},
  {2029, "NNPDF30_nlo_nf_4_pdfas_29"},
  {2030, "NNPDF30_nlo_nf_4_pdfas_30"},
  {2031, "NNPDF30_nlo_nf_4_pdfas_31"},
  {2032, "NNPDF30_nlo_nf_4_pdfas_32"},
  {2033, "NNPDF30_nlo_nf_4_pdfas_33"},
  {2034, "NNPDF30_nlo_nf_4_pdfas_34"},
  {2035, "NNPDF30_nlo_nf_4_pdfas_35"},
  {2036, "NNPDF30_nlo_nf_4_pdfas_36"},
  {2037, "NNPDF30_nlo_nf_4_pdfas_37"},
  {2038, "NNPDF30_nlo_nf_4_pdfas_38"},
  {2039, "NNPDF30_nlo_nf_4_pdfas_39"},
  {2040, "NNPDF30_nlo_nf_4_pdfas_40"},
  {2041, "NNPDF30_nlo_nf_4_pdfas_41"},
  {2042, "NNPDF30_nlo_nf_4_pdfas_42"},
  {2043, "NNPDF30_nlo_nf_4_pdfas_43"},
  {2044, "NNPDF30_nlo_nf_4_pdfas_44"},
  {2045, "NNPDF30_nlo_nf_4_pdfas_45"},
  {2046, "NNPDF30_nlo_nf_4_pdfas_46"},
  {2047, "NNPDF30_nlo_nf_4_pdfas_47"},
  {2048, "NNPDF30_nlo_nf_4_pdfas_48"},
  {2049, "NNPDF30_nlo_nf_4_pdfas_49"},
  {2050, "NNPDF30_nlo_nf_4_pdfas_50"},
  {2051, "NNPDF30_nlo_nf_4_pdfas_51"},
  {2052, "NNPDF30_nlo_nf_4_pdfas_52"},
  {2053, "NNPDF30_nlo_nf_4_pdfas_53"},
  {2054, "NNPDF30_nlo_nf_4_pdfas_54"},
  {2055, "NNPDF30_nlo_nf_4_pdfas_55"},
  {2056, "NNPDF30_nlo_nf_4_pdfas_56"},
  {2057, "NNPDF30_nlo_nf_4_pdfas_57"},
  {2058, "NNPDF30_nlo_nf_4_pdfas_58"},
  {2059, "NNPDF30_nlo_nf_4_pdfas_59"},
  {2060, "NNPDF30_nlo_nf_4_pdfas_60"},
  {2061, "NNPDF30_nlo_nf_4_pdfas_61"},
  {2062, "NNPDF30_nlo_nf_4_pdfas_62"},
  {2063, "NNPDF30_nlo_nf_4_pdfas_63"},
  {2064, "NNPDF30_nlo_nf_4_pdfas_64"},
  {2065, "NNPDF30_nlo_nf_4_pdfas_65"},
  {2066, "NNPDF30_nlo_nf_4_pdfas_66"},
  {2067, "NNPDF30_nlo_nf_4_pdfas_67"},
  {2068, "NNPDF30_nlo_nf_4_pdfas_68"},
  {2069, "NNPDF30_nlo_nf_4_pdfas_69"},
  {2070, "NNPDF30_nlo_nf_4_pdfas_70"},
  {2071, "NNPDF30_nlo_nf_4_pdfas_71"},
  {2072, "NNPDF30_nlo_nf_4_pdfas_72"},
  {2073, "NNPDF30_nlo_nf_4_pdfas_73"},
  {2074, "NNPDF30_nlo_nf_4_pdfas_74"},
  {2075, "NNPDF30_nlo_nf_4_pdfas_75"},
  {2076, "NNPDF30_nlo_nf_4_pdfas_76"},
  {2077, "NNPDF30_nlo_nf_4_pdfas_77"},
  {2078, "NNPDF30_nlo_nf_4_pdfas_78"},
  {2079, "NNPDF30_nlo_nf_4_pdfas_79"},
  {2080, "NNPDF30_nlo_nf_4_pdfas_80"},
  {2081, "NNPDF30_nlo_nf_4_pdfas_81"},
  {2082, "NNPDF30_nlo_nf_4_pdfas_82"},
  {2083, "NNPDF30_nlo_nf_4_pdfas_83"},
  {2084, "NNPDF30_nlo_nf_4_pdfas_84"},
  {2085, "NNPDF30_nlo_nf_4_pdfas_85"},
  {2086, "NNPDF30_nlo_nf_4_pdfas_86"},
  {2087, "NNPDF30_nlo_nf_4_pdfas_87"},
  {2088, "NNPDF30_nlo_nf_4_pdfas_88"},
  {2089, "NNPDF30_nlo_nf_4_pdfas_89"},
  {2090, "NNPDF30_nlo_nf_4_pdfas_90"},
  {2091, "NNPDF30_nlo_nf_4_pdfas_91"},
  {2092, "NNPDF30_nlo_nf_4_pdfas_92"},
  {2093, "NNPDF30_nlo_nf_4_pdfas_93"},
  {2094, "NNPDF30_nlo_nf_4_pdfas_94"},
  {2095, "NNPDF30_nlo_nf_4_pdfas_95"},
  {2096, "NNPDF30_nlo_nf_4_pdfas_96"},
  {2097, "NNPDF30_nlo_nf_4_pdfas_97"},
  {2098, "NNPDF30_nlo_nf_4_pdfas_98"},
  {2099, "NNPDF30_nlo_nf_4_pdfas_99"},
  {2100, "NNPDF30_nlo_nf_4_pdfas_100"},
  {2101, "NNPDF30_nlo_nf_4_pdfas_101"},
  {2102, "NNPDF30_nlo_nf_4_pdfas_102"},

  {2103, "CT14nlo_NF4_0"},

  {2104, "CT10nlo_nf4_0"},
  {2105, "CT10nlo_nf4_1"},

  {3001, "CT10nlo_0"},
  {3002, "CT10nlo_1"},
  {3003, "CT10nlo_2"},
  {3004, "CT10nlo_3"},
  {3005, "CT10nlo_4"},
  {3006, "CT10nlo_5"},
  {3007, "CT10nlo_6"},
  {3008, "CT10nlo_7"},
  {3009, "CT10nlo_8"},
  {3010, "CT10nlo_9"},
  {3011, "CT10nlo_10"},
  {3012, "CT10nlo_11"},
  {3013, "CT10nlo_12"},
  {3014, "CT10nlo_13"},
  {3015, "CT10nlo_14"},
  {3016, "CT10nlo_15"},
  {3017, "CT10nlo_16"},
  {3018, "CT10nlo_17"},
  {3019, "CT10nlo_18"},
  {3020, "CT10nlo_19"},
  {3021, "CT10nlo_20"},
  {3022, "CT10nlo_21"},
  {3023, "CT10nlo_22"},
  {3024, "CT10nlo_23"},
  {3025, "CT10nlo_24"},
  {3026, "CT10nlo_25"},
  {3027, "CT10nlo_26"},
  {3028, "CT10nlo_27"},
  {3029, "CT10nlo_28"},
  {3030, "CT10nlo_29"},
  {3031, "CT10nlo_30"},
  {3032, "CT10nlo_31"},
  {3033, "CT10nlo_32"},
  {3034, "CT10nlo_33"},
  {3035, "CT10nlo_34"},
  {3036, "CT10nlo_35"},
  {3037, "CT10nlo_36"},
  {3038, "CT10nlo_37"},
  {3039, "CT10nlo_38"},
  {3040, "CT10nlo_39"},
  {3041, "CT10nlo_40"},
  {3042, "CT10nlo_41"},
  {3043, "CT10nlo_42"},
  {3044, "CT10nlo_43"},
  {3045, "CT10nlo_44"},
  {3046, "CT10nlo_45"},
  {3047, "CT10nlo_46"},
  {3048, "CT10nlo_47"},
  {3049, "CT10nlo_48"},
  {3050, "CT10nlo_49"},
  {3051, "CT10nlo_50"},
  {3052, "CT10nlo_51"},
  {3053, "CT10nlo_52"},

  {3054, "CT10nlo_as_0117_0"},

  {3055, "CT10nlo_as_0119_0"},

  {4001, "MMHT2014nlo68clas118_0"},
  {4002, "MMHT2014nlo68clas118_1"},
  {4003, "MMHT2014nlo68clas118_2"},
  {4004, "MMHT2014nlo68clas118_3"},
  {4005, "MMHT2014nlo68clas118_4"},
  {4006, "MMHT2014nlo68clas118_5"},
  {4007, "MMHT2014nlo68clas118_6"},
  {4008, "MMHT2014nlo68clas118_7"},
  {4009, "MMHT2014nlo68clas118_8"},
  {4010, "MMHT2014nlo68clas118_9"},
  {4011, "MMHT2014nlo68clas118_10"},
  {4012, "MMHT2014nlo68clas118_11"},
  {4013, "MMHT2014nlo68clas118_12"},
  {4014, "MMHT2014nlo68clas118_13"},
  {4015, "MMHT2014nlo68clas118_14"},
  {4016, "MMHT2014nlo68clas118_15"},
  {4017, "MMHT2014nlo68clas118_16"},
  {4018, "MMHT2014nlo68clas118_17"},
  {4019, "MMHT2014nlo68clas118_18"},
  {4020, "MMHT2014nlo68clas118_19"},
  {4021, "MMHT2014nlo68clas118_20"},
  {4022, "MMHT2014nlo68clas118_21"},
  {4023, "MMHT2014nlo68clas118_22"},
  {4024, "MMHT2014nlo68clas118_23"},
  {4025, "MMHT2014nlo68clas118_24"},
  {4026, "MMHT2014nlo68clas118_25"},
  {4027, "MMHT2014nlo68clas118_26"},
  {4028, "MMHT2014nlo68clas118_27"},
  {4029, "MMHT2014nlo68clas118_28"},
  {4030, "MMHT2014nlo68clas118_29"},
  {4031, "MMHT2014nlo68clas118_30"},
  {4032, "MMHT2014nlo68clas118_31"},
  {4033, "MMHT2014nlo68clas118_32"},
  {4034, "MMHT2014nlo68clas118_33"},
  {4035, "MMHT2014nlo68clas118_34"},
  {4036, "MMHT2014nlo68clas118_35"},
  {4037, "MMHT2014nlo68clas118_36"},
  {4038, "MMHT2014nlo68clas118_37"},
  {4039, "MMHT2014nlo68clas118_38"},
  {4040, "MMHT2014nlo68clas118_39"},
  {4041, "MMHT2014nlo68clas118_40"},
  {4042, "MMHT2014nlo68clas118_41"},
  {4043, "MMHT2014nlo68clas118_42"},
  {4044, "MMHT2014nlo68clas118_43"},
  {4045, "MMHT2014nlo68clas118_44"},
  {4046, "MMHT2014nlo68clas118_45"},
  {4047, "MMHT2014nlo68clas118_46"},
  {4048, "MMHT2014nlo68clas118_47"},
  {4049, "MMHT2014nlo68clas118_48"},
  {4050, "MMHT2014nlo68clas118_49"},
  {4051, "MMHT2014nlo68clas118_50"},

  {4052, "MMHT2014nlo_asmzsmallrange_0"},
  {4053, "MMHT2014nlo_asmzsmallrange_1"},
  {4054, "MMHT2014nlo_asmzsmallrange_2"},
  {4055, "MMHT2014nlo_asmzsmallrange_3"},
  {4056, "MMHT2014nlo_asmzsmallrange_4"},

  {4057, "MSTW2008nlo68cl_nf4_0"},
  {4058, "MSTW2008nlo68cl_nf4_1"},
  {4059, "MSTW2008nlo68cl_nf4_2"},
  {4060, "MSTW2008nlo68cl_nf4_3"},
  {4061, "MSTW2008nlo68cl_nf4_4"},
  {4062, "MSTW2008nlo68cl_nf4_5"},
  {4063, "MSTW2008nlo68cl_nf4_6"},
  {4064, "MSTW2008nlo68cl_nf4_7"},
  {4065, "MSTW2008nlo68cl_nf4_8"},
  {4066, "MSTW2008nlo68cl_nf4_9"},
  {4067, "MSTW2008nlo68cl_nf4_10"},
  {4068, "MSTW2008nlo68cl_nf4_11"},
  {4069, "MSTW2008nlo68cl_nf4_12"},
  {4070, "MSTW2008nlo68cl_nf4_13"},
  {4071, "MSTW2008nlo68cl_nf4_14"},
  {4072, "MSTW2008nlo68cl_nf4_15"},
  {4073, "MSTW2008nlo68cl_nf4_16"},
  {4074, "MSTW2008nlo68cl_nf4_17"},
  {4075, "MSTW2008nlo68cl_nf4_18"},
  {4076, "MSTW2008nlo68cl_nf4_19"},
  {4077, "MSTW2008nlo68cl_nf4_20"},
  {4078, "MSTW2008nlo68cl_nf4_21"},
  {4079, "MSTW2008nlo68cl_nf4_22"},
  {4080, "MSTW2008nlo68cl_nf4_23"},
  {4081, "MSTW2008nlo68cl_nf4_24"},
  {4082, "MSTW2008nlo68cl_nf4_25"},
  {4083, "MSTW2008nlo68cl_nf4_26"},
  {4084, "MSTW2008nlo68cl_nf4_27"},
  {4085, "MSTW2008nlo68cl_nf4_28"},
  {4086, "MSTW2008nlo68cl_nf4_29"},
  {4087, "MSTW2008nlo68cl_nf4_30"},
  {4088, "MSTW2008nlo68cl_nf4_31"},
  {4089, "MSTW2008nlo68cl_nf4_32"},
  {4090, "MSTW2008nlo68cl_nf4_33"},
  {4091, "MSTW2008nlo68cl_nf4_34"},
  {4092, "MSTW2008nlo68cl_nf4_35"},
  {4093, "MSTW2008nlo68cl_nf4_36"},
  {4094, "MSTW2008nlo68cl_nf4_37"},
  {4095, "MSTW2008nlo68cl_nf4_38"},
  {4096, "MSTW2008nlo68cl_nf4_39"},
  {4097, "MSTW2008nlo68cl_nf4_40"}
};

std::map<int,std::string> LHEweightMAP9 = {
  {10001, "Q2_0"},
  {10002, "Q2_1"},
  {10003, "Q2_2"},
  {10004, "Q2_3"},
  {10005, "Q2_4"},
  {10006, "Q2_5"},
  {10007, "Q2_6"},
  {10008, "Q2_7"},
  {10009, "Q2_8"}
};

std::map<int,std::string>& getNanoAODQ2ScaleMap()
{
  return LHEweightMAPnanoAOD;
}

std::map<int,std::string>& getLHEMap(int numWeights)
{
  if(numWeights <= 0)
    return emptyMap;
  else if(numWeights == 9)
    return LHEweightMAP9;
  else if(numWeights <= 111)
    return LHEweightMAP111;
  else if(numWeights <= 254)
    return LHEweightMAP254;
  else if(numWeights <= 266) // Some of these events do not have all weights (why?), so we use the <= intead of ==
    return LHEweightMAP266;
  else if(numWeights <= 446)
    return LHEweightMAP446;
  return emptyMap;
}
