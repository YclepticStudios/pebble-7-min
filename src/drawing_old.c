// #include <pebble.h>
// #include "drawing.h"
// #include "main.h"

// // constants
// #define CIR_X 72
// #ifdef PBL_SDK_3
// #define CIR_Y 84
// #else
// #define CIR_Y 84 - 15
// #endif
// #define CIR_R 60
// #define SCREEN_WIDTH 144
// #define SCREEN_HEIGHT 168



// // ********** Data Variables ************ //
// // text descriptions
// static const char *TXT_Current[] = {"First: Jumping Jacks", "Rest", "Jumping Jacks", "Next: Wall Sit", "Wall Sit", "Next: Pushups", "Pushups", "Next: Crunches", "Crunches", "Next: Step Ups", "Step Ups", "Next: Squats", "Squats", "Next: Chair Dips", "Chair Dips", "Next: Planks", "Planks", "Next: High Knees", "High Knees", "Next: Lunges", "Lunges", "Next: Pushups & Rotations", "Pushups & Rotations", "Next: Side Planks", "Side Planks", "Done!", "Done!"};

// // button data
// uint8_t drawing_BT_State = 0;
// static const GPoint BT_Play[] = {{CIR_X + 40, CIR_Y}, {CIR_X - 20, CIR_Y + 34}, {CIR_X - 20, CIR_Y}, {CIR_X - 20, CIR_Y - 34}};
// static const GPoint BT_Pause_L[] = {{CIR_X - 30, CIR_Y - 30}, {CIR_X - 30, CIR_Y + 30}, {CIR_X - 10, CIR_Y + 30}, {CIR_X - 10, CIR_Y - 30}};
// static const GPoint BT_Pause_R[] = {{CIR_X + 10, CIR_Y - 30}, {CIR_X + 30, CIR_Y - 30}, {CIR_X + 30, CIR_Y + 30}, {CIR_X + 10, CIR_Y + 30}};
// GPoint BT_Left[] = {{CIR_X + 40, CIR_Y}, {CIR_X - 20, CIR_Y + 34}, {CIR_X - 20, CIR_Y}, {CIR_X - 20, CIR_Y - 34}};
// GPoint BT_Right[] = {{CIR_X + 40, CIR_Y}, {CIR_X - 20, CIR_Y + 34}, {CIR_X - 20, CIR_Y}, {CIR_X - 20, CIR_Y - 34}};

// // person data
// bool drawing_Reversed = false;
// int8_t drawing_Stance = 0, drawing_Index = 0;
// struct HumanBody {
//     GPoint head;
//     GPoint chest;
//     GPoint r_elbo;
//     GPoint r_hand;
//     GPoint l_elbo;
//     GPoint l_hand;
//     GPoint waist;
//     GPoint r_knee;
//     GPoint r_foot;
//     GPoint l_knee;
//     GPoint l_foot;
//     uint32_t duration, delay;
//     AnimationCurve curve;
// };
// // data for poses (two values for each, x and y): head, chest, r_elbo, r_hand, l_elbo, l_hand, waist, r_knee, r_foot, l_knee, l_foot
// static const uint8_t Pose_Count[] = {35, 4, 6, 7, 4, 4, 6, 7, 4, 9, 2, 5, 4, 6, 16};
// static const struct HumanBody Pose_Standing[35] = {
//         // wave
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 700, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 700, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseInOut},
//         {{0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseInOut},
//         {{0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseInOut},
//         {{0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 150, 0, AnimationCurveEaseInOut},
//         // walking
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseInOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 1000, 0, AnimationCurveEaseInOut}, // pause
//         // stride
//         {{0, -30}, {0, -10}, {-9, -1}, {-10, 13}, {5, 2}, {18, 6}, {0, 15}, {10, 27}, {16, 45}, {-4, 30}, {-17, 44}, 200, 0, AnimationCurveEaseIn},
//         {{0, -32}, {0, -12}, {-3, 4}, {4, 13}, {-5, 5}, {-5, 17}, {0, 13}, {0, 31}, {-3, 49}, {3, 30}, {-7, 44}, 200, 0, AnimationCurveLinear},
//         {{0, -30}, {0, -10}, {5, 2}, {18, 6}, {-9, -1}, {-10, 13}, {0, 15}, {-4, 30}, {-17, 44}, {10, 27}, {16, 45}, 200, 0, AnimationCurveLinear},
//         {{0, -32}, {0, -12}, {-5, 5}, {-5, 17}, {-3, 4}, {4, 13}, {0, 13}, {3, 30}, {-7, 44}, {0, 31}, {-3, 49}, 200, 0, AnimationCurveLinear},
//         // stride
//         {{0, -30}, {0, -10}, {-9, -1}, {-10, 13}, {5, 2}, {18, 6}, {0, 15}, {10, 27}, {16, 45}, {-4, 30}, {-17, 44}, 200, 0, AnimationCurveLinear},
//         {{0, -32}, {0, -12}, {-3, 4}, {4, 13}, {-5, 5}, {-5, 17}, {0, 13}, {0, 31}, {-3, 49}, {3, 30}, {-7, 44}, 200, 0, AnimationCurveLinear},
//         {{0, -30}, {0, -10}, {5, 2}, {18, 6}, {-9, -1}, {-10, 13}, {0, 15}, {-4, 30}, {-17, 44}, {10, 27}, {16, 45}, 200, 0, AnimationCurveLinear},
//         {{0, -32}, {0, -12}, {-5, 5}, {-5, 17}, {-3, 4}, {4, 13}, {0, 13}, {3, 30}, {-7, 44}, {0, 31}, {-3, 49}, 200, 0, AnimationCurveLinear},
//         // wave
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 700, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 700, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseInOut},
//         {{0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseInOut},
//         {{0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseInOut},
//         {{0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 150, 0, AnimationCurveEaseInOut},
//         // walking back
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseInOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 500, 0, AnimationCurveEaseInOut}, // pause
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 500, 0, AnimationCurveEaseInOut}, // pause
//         // stride
//         {{0, -30}, {0, -10}, {9, -1}, {10, 13}, {-5, 2}, {-18, 6}, {0, 15}, {-10, 27}, {-16, 45}, {4, 30}, {17, 44}, 200, 0, AnimationCurveEaseIn},
//         {{0, -32}, {0, -12}, {3, 4}, {-4, 13}, {5, 5}, {5, 17}, {0, 13}, {-0, 31}, {3, 49}, {-3, 30}, {7, 44}, 200, 0, AnimationCurveLinear},
//         {{0, -30}, {0, -10}, {-5, 2}, {-18, 6}, {9, -1}, {10, 13}, {0, 15}, {4, 30}, {17, 44}, {-10, 27}, {-16, 45}, 200, 0, AnimationCurveLinear},
//         {{0, -32}, {0, -12}, {5, 5}, {5, 17}, {3, 4}, {-4, 13}, {0, 13}, {-3, 30}, {7, 44}, {-0, 31}, {3, 49}, 200, 0, AnimationCurveLinear},
//         // stride
//         {{0, -30}, {0, -10}, {9, -1}, {10, 13}, {-5, 2}, {-18, 6}, {0, 15}, {-10, 27}, {-16, 45}, {4, 30}, {17, 44}, 200, 0, AnimationCurveLinear},
//         {{0, -32}, {0, -12}, {3, 4}, {-4, 13}, {5, 5}, {5, 17}, {0, 13}, {-0, 31}, {3, 49}, {-3, 30}, {7, 44}, 200, 0, AnimationCurveLinear},
//         {{0, -30}, {0, -10}, {-5, 2}, {-18, 6}, {9, -1}, {10, 13}, {0, 15}, {4, 30}, {17, 44}, {-10, 27}, {-16, 45}, 200, 0, AnimationCurveLinear},
//         {{0, -32}, {0, -12}, {5, 5}, {5, 17}, {3, 4}, {-4, 13}, {0, 13}, {-3, 30}, {7, 44}, {-0, 31}, {3, 49}, 200, 0, AnimationCurveLinear},
// };
// static const struct HumanBody Pose_Resting[4] = {
//         // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-7, 28}, {-10, 44}, {7, 28}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-7, 28}, {-10, 44}, {7, 28}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // pause
//         // shrugging
//         {{0, -31}, {0, -11}, {-12, -9}, {-17, 2}, {12, -9}, {17, 2}, {0, 14}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // shrug
//         {{0, -31}, {0, -11}, {-12, -9}, {-17, 2}, {12, -9}, {17, 2}, {0, 14}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // pause
// };
// static const struct HumanBody Pose_JumpingJacks[6] = {
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseIn}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 150, 0, AnimationCurveEaseIn}, // delay
//         {{0, -35}, {0, -15}, {-12, -17}, {-24, -19}, {12, -17}, {24, -19}, {0, 10}, {-11, 19}, {-17, 32}, {11, 19}, {17, 32}, 200, 0, AnimationCurveEaseOut},
//         {{0, -26}, {0, -6}, {-11, -7}, {-21, -16}, {11, -7}, {21, -16}, {0, 19}, {-10, 31}, {-20, 44}, {10, 31}, {20, 44}, 200, 0, AnimationCurveEaseIn},
//         {{0, -26}, {0, -6}, {-11, -7}, {-21, -16}, {11, -7}, {21, -16}, {0, 19}, {-10, 31}, {-20, 44}, {10, 31}, {20, 44}, 150, 0, AnimationCurveEaseIn}, // delay
//         {{0, -35}, {0, -15}, {-12, -17}, {-24, -19}, {12, -17}, {24, -19}, {0, 10}, {-11, 19}, {-17, 32}, {11, 19}, {17, 32}, 200, 0, AnimationCurveEaseOut},
// };
// static const struct HumanBody Pose_WallSit[7] = {
//         {{5, -20}, {0, 0}, {7, 10}, {17, 15}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 400, 0, AnimationCurveEaseInOut}, // sitting
//         {{5, -20}, {0, 0}, {7, 10}, {17, 15}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 500, 0, AnimationCurveEaseInOut}, // pause
//         {{5, -20}, {0, 0}, {7, 10}, {17, 15}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 500, 0, AnimationCurveEaseInOut}, // pause
//         {{5, -20}, {0, 0}, {7, 10}, {17, 15}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 500, 0, AnimationCurveEaseInOut}, // pause
//         {{5, -20}, {0, 0}, {7, 10}, {17, 15}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 500, 0, AnimationCurveEaseInOut}, // pause
//         {{7, -18}, {0, 0}, {12, 0}, {20, -9}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 400, 0, AnimationCurveEaseInOut}, // forehead wipe
//         {{7, -18}, {0, 0}, {12, 0}, {20, -9}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_Pushups[4] = {
//         {{28, 12}, {6, 12}, {-2, 19}, {7, 22}, {-2, 19}, {7, 22}, {-9, 14}, {-32, 20}, {-47, 23}, {-32, 20}, {-47, 23}, 400, 0, AnimationCurveEaseInOut}, // lying
//         {{28, 12}, {6, 12}, {-2, 19}, {7, 22}, {-2, 19}, {7, 22}, {-9, 14}, {-32, 20}, {-47, 23}, {-32, 20}, {-47, 23}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{25, -4}, {4, 0}, {5, 11}, {7, 22}, {5, 11}, {7, 22}, {-21, 7}, {-33, 16}, {-47, 23}, {-33, 16}, {-47, 23}, 400, 0, AnimationCurveEaseInOut}, // pushup
//         {{25, -4}, {4, 0}, {5, 11}, {7, 22}, {5, 11}, {7, 22}, {-21, 7}, {-33, 16}, {-47, 23}, {-33, 16}, {-47, 23}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_Crunches[4] = {
//         {{-26, 19}, {-4, 21}, {-14, 26}, {-23, 18}, {-14, 26}, {-23, 18}, {20, 21}, {21, 6}, {36, 6}, {21, 6}, {36, 6}, 400, 0, AnimationCurveEaseInOut}, // lying
//         {{-26, 19}, {-4, 21}, {-14, 26}, {-23, 18}, {-14, 26}, {-23, 18}, {20, 21}, {21, 6}, {36, 6}, {21, 6}, {36, 6}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{-17, 5}, {-4, 20}, {3, 11}, {-5, 2}, {3, 11}, {-5, 2}, {20, 21}, {17, 6}, {29, 1}, {17, 6}, {29, 1}, 400, 0, AnimationCurveEaseInOut}, // chrunch
//         {{-17, 5}, {-4, 20}, {3, 11}, {-5, 2}, {3, 11}, {-5, 2}, {20, 21}, {17, 6}, {29, 1}, {17, 6}, {29, 1}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_SrepUps[6] = {
//         {{-14, -19}, {-14, 0}, {-13, 11}, {-9, 21}, {-13, 11}, {-9, 21}, {-14, 23}, {-13, 36}, {-14, 50}, {-13, 36}, {-14, 50}, 400, 0, AnimationCurveEaseInOut}, // lying
//         {{-14, -19}, {-14, 0}, {-13, 11}, {-9, 21}, {-13, 11}, {-9, 21}, {-14, 23}, {-13, 36}, {-14, 50}, {-13, 36}, {-14, 50}, 400, 0, AnimationCurveEaseInOut}, // lying
//         {{5, -16}, {-2, 3}, {6, 9}, {16, 6}, {6, 9}, {16, 6}, {-10, 23}, {4, 19}, {7, 32}, {-11, 36}, {-13, 50}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{7, -37}, {7, -18}, {8, -6}, {13, 3}, {8, -6}, {13, 3}, {7, 5}, {8, 18}, {7, 32}, {8, 18}, {7, 32}, 400, 0, AnimationCurveEaseInOut}, // chrunch
//         {{7, -37}, {7, -18}, {8, -6}, {13, 3}, {8, -6}, {13, 3}, {7, 5}, {8, 18}, {7, 32}, {8, 18}, {7, 32}, 400, 0, AnimationCurveEaseInOut}, // chrunch
//         {{5, -16}, {-2, 3}, {6, 9}, {16, 6}, {6, 9}, {16, 6}, {-10, 23}, {4, 19}, {7, 32}, {-11, 36}, {-13, 50}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_Squats[7] = {
//         {{-4, -30}, {-4, -10}, {7, -6}, {19, -7}, {7, -6}, {19, -7}, {-4, 18}, {-2, 31}, {-4, 46}, {-2, 31}, {-4, 46}, 600, 0, AnimationCurveEaseInOut}, // standing
//         {{-4, -30}, {-4, -10}, {7, -6}, {19, -7}, {7, -6}, {19, -7}, {-4, 18}, {-2, 31}, {-4, 46}, {-2, 31}, {-4, 46}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{-4, -30}, {-4, -10}, {7, -6}, {19, -7}, {7, -6}, {19, -7}, {-4, 18}, {-2, 31}, {-4, 46}, {-2, 31}, {-4, 46}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{3, -9}, {-5, 12}, {6, 13}, {17, 12}, {6, 13}, {17, 12}, {-12, 30}, {7, 31}, {-4, 46}, {7, 31}, {-4, 46}, 600, 0, AnimationCurveEaseInOut}, // crouched
//         {{3, -9}, {-5, 12}, {6, 13}, {17, 12}, {6, 13}, {17, 12}, {-12, 30}, {7, 31}, {-4, 46}, {7, 31}, {-4, 46}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{3, -9}, {-5, 12}, {6, 13}, {17, 12}, {6, 13}, {17, 12}, {-12, 30}, {7, 31}, {-4, 46}, {7, 31}, {-4, 46}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{3, -9}, {-5, 12}, {6, 13}, {17, 12}, {6, 13}, {17, 12}, {-12, 30}, {7, 31}, {-4, 46}, {7, 31}, {-4, 46}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_ChairDips[4] = {
//         {{2, -25}, {0, -4}, {4, 6}, {6, 18}, {4, 6}, {6, 18}, {-4, 19}, {-20, 20}, {-20, 37}, {-20, 20}, {-20, 37}, 600, 0, AnimationCurveEaseInOut}, // sitting
//         {{2, -25}, {0, -4}, {4, 6}, {6, 18}, {4, 6}, {6, 18}, {-4, 19}, {-20, 20}, {-20, 37}, {-20, 20}, {-20, 37}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{0, -13}, {-2, 6}, {9, 8}, {6, 18}, {9, 8}, {6, 18}, {-6, 31}, {-20, 19}, {-20, 37}, {-20, 19}, {-20, 37}, 600, 0, AnimationCurveEaseInOut}, // squat
//         {{0, -13}, {-2, 6}, {9, 8}, {6, 18}, {9, 8}, {6, 18}, {-6, 31}, {-20, 19}, {-20, 37}, {-20, 19}, {-20, 37}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_Planks[9] = {
//         {{29, 1}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // laying
//         {{29, 1}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{29, 1}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{29, 1}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{29, 1}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{30, 6}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // head down
//         {{30, 6}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{30, 6}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{30, 6}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_HighKnees[2] = {
//         {{-3, -30}, {-3, -7}, {-10, 2}, {1, 7}, {0, 4}, {11, 0}, {-3, 17}, {12, 14}, {4, 24}, {-4, 32}, {-6, 46}, 330, 0, AnimationCurveLinear}, // running
//         {{-3, -29}, {-3, -7}, {0, 4}, {11, 0}, {-10, 2}, {1, 7}, {-3, 17}, {-4, 32}, {-6, 46}, {12, 14}, {4, 24}, 330, 0, AnimationCurveLinear}, // running
// };
// static const struct HumanBody Pose_Lunges[5] = {
//         {{0, -32}, {0, -10}, {-9, -2}, {0, 6}, {-9, -2}, {0, 6}, {0, 16}, {2, 30}, {0, 45}, {2, 30}, {0, 45}, 400, 0, AnimationCurveEaseInOut}, // upright
//         {{0, -32}, {0, -10}, {-9, -2}, {0, 6}, {-9, -2}, {0, 6}, {0, 16}, {2, 30}, {0, 45}, {2, 30}, {0, 45}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{0, -29}, {0, -7}, {-9, 1}, {0, 9}, {-9, 1}, {0, 9}, {0, 19}, {7, 25}, {9, 40}, {2, 30}, {0, 45}, 80, 0, AnimationCurveEaseIn}, // raise foot
//         {{0, -21}, {0, 0}, {-9, 7}, {0, 15}, {-9, 7}, {0, 15}, {0, 25}, {15, 28}, {15, 45}, {-3, 39}, {-17, 45}, 400, 0, AnimationCurveEaseOut}, // crouched
//         {{0, -21}, {0, 0}, {-9, 7}, {0, 15}, {-9, 7}, {0, 15}, {0, 25}, {15, 28}, {15, 45}, {-3, 39}, {-17, 45}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_PushupRots[4] = {
//         {{28, 13}, {6, 13}, {-2, 20}, {7, 23}, {-2, 20}, {7, 23}, {-9, 13}, {-32, 17}, {-47, 19}, {-32, 17}, {-47, 19}, 400, 0, AnimationCurveEaseInOut}, // lying
//         {{28, 13}, {6, 13}, {-2, 20}, {7, 23}, {-2, 20}, {7, 23}, {-9, 13}, {-32, 17}, {-47, 19}, {-32, 17}, {-47, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{25, -3}, {4, 1}, {3, 4}, {4, -21}, {5, 12}, {7, 23}, {-21, 6}, {-33, 13}, {-47, 19}, {-33, 13}, {-47, 19}, 400, 0, AnimationCurveEaseInOut}, // pushup
//         {{25, -3}, {4, 1}, {3, 4}, {4, -21}, {5, 12}, {7, 23}, {-21, 6}, {-33, 13}, {-47, 19}, {-33, 13}, {-47, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_SidePlank[6] = {
//         {{29, 2}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}, 400, 0, AnimationCurveEaseInOut}, // lying
//         {{29, 2}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{29, 4}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}, 400, 0, AnimationCurveEaseInOut}, // waving
//         {{29, 4}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{29, 4}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
//         {{29, 4}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}, 400, 0, AnimationCurveEaseInOut}, // pause
// };
// static const struct HumanBody Pose_Celebrating[16] = {
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-11, -12}, {-20, -22}, {11, -12}, {20, -22}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // raised hands
//         {{0, -30}, {0, -10}, {-11, -12}, {-20, -22}, {11, -12}, {20, -22}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{5, -29}, {3, -8}, {-8, -12}, {-18, -21}, {15, -4}, {6, -13}, {2, 17}, {-6, 29}, {-10, 44}, {8, 31}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // victory stance
//         {{5, -29}, {3, -8}, {-8, -12}, {-18, -21}, {15, -4}, {6, -13}, {2, 17}, {-6, 29}, {-10, 44}, {8, 31}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-11, -12}, {-20, -22}, {11, -12}, {20, -22}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // raised hands
//         {{0, -30}, {0, -10}, {-11, -12}, {-20, -22}, {11, -12}, {20, -22}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // standing
//         {{0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
//         {{-5, -29}, {-3, -8}, {-15, -4}, {-6, -13}, {8, -12}, {18, -21}, {-2, 17}, {-8, 31}, {-10, 44}, {6, 29}, {10, 44}, 400, 0, AnimationCurveEaseOut}, // victory stance 2
//         {{-5, -29}, {-3, -8}, {-15, -4}, {-6, -13}, {8, -12}, {18, -21}, {-2, 17}, {-8, 31}, {-10, 44}, {6, 29}, {10, 44}, 200, 0, AnimationCurveEaseOut}, // pause
// };
// // list of poses
// static const struct HumanBody *Pose_List[] = { Pose_Standing, Pose_Resting, Pose_JumpingJacks, Pose_WallSit, Pose_Pushups, Pose_Crunches, Pose_SrepUps, Pose_Squats, Pose_ChairDips, Pose_Planks, Pose_HighKnees, Pose_Lunges, Pose_PushupRots, Pose_SidePlank, Pose_Celebrating};
// // pose variables
// struct HumanBody oldPerson, person, newPerson;

// // functions
// struct HumanBody data2screen(struct HumanBody body);
// void drawing_set_pose(struct HumanBody *body, int8_t *pose);
// static void ani_next_pose(void);



// // ********** General Drawing ********** //
// // draw progress ring color (perc = 0-96)
// int32_t delta, r, g, b;
// GColor col1, col2;
// int map[16] = { 1,13,4,16, 9,5,12,8, 3,15,2,14, 11,7,10,6 };
// uint8_t data[16];
// GBitmap *img_grid;
// #ifdef PBL_COLOR
// void drawing_BackColor(GContext *ctx, int16_t perc){
//     // get first color
//     r = (perc / 16) * 85;
//     r = (r > 255) ? 255 : r;
//     g = ((95 - perc) / 16 + 1) * 85;
//     g = (g > 255) ? 255 : g;
//     col1 = GColorFromRGB(r, g, 0);
//     // get second color
//     r = (perc / 16 + 1) * 85;
//     r = (r > 255) ? 255 : r;
//     g = ((95 - perc) / 16) * 85;
//     g = (g > 255) ? 255 : g;
//     col2 = GColorFromRGB(r, g, 0);
//     // copy data
//     for (uint8_t ii = 0; ii < 16; ii++)
//     data[ii] = (perc % 16 < map[ii]) ? col1.argb : col2.argb;
//     // set data
//     gbitmap_set_data(img_grid, data, GBitmapFormat8Bit, 4, false);
//     // draw image
//     graphics_draw_bitmap_in_rect(ctx, img_grid, GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
//     // return
// }
// #endif
// // get point from angle
// GPoint getAnglePoint(int32_t ang){
//   // get the point locations
//   int32_t Y = (-cos_lookup(ang) * SCREEN_HEIGHT / TRIG_MAX_RATIO) + (int32_t)(CIR_Y);
//   int32_t X = (-sin_lookup(ang) * SCREEN_HEIGHT / TRIG_MAX_RATIO) + (int32_t)(CIR_X);
//   // add them to the path
//   return GPoint(X, Y);
// }
// // draw overlay
// void drawing_Progress(GContext *ctx, int32_t angle){
//   // set the fill color
// #ifdef PBL_COLOR
//   graphics_context_set_fill_color(ctx, GColorDarkGray);
// #else
//   graphics_context_set_fill_color(ctx, GColorBlack);
// #endif

//   // loop and get points for black covering
//   int8_t idx = 0;
//   int32_t step = angle / (4);
//   if (step < 1) step = 1;
//   GPoint cirPoints[8];
//   for (int32_t tAng = 0; tAng < angle; tAng += step){
//     cirPoints[idx++] = getAnglePoint(tAng);
//   }
//   // add point at hand position, and in center (to form pie wedge)
//   cirPoints[idx++] = getAnglePoint(angle);
//   cirPoints[idx++] = GPoint(CIR_X, CIR_Y);

//   // draw black covering
//   GPathInfo info;
//   GPath *path;
//   info.num_points = idx;
//   info.points = cirPoints;
//   path = gpath_create(&info);
//   gpath_draw_filled(ctx, path);
//   gpath_destroy(path);
// }
// // draw background
// static bool onetimeVib = false;
// static int32_t progressAngle = 0;
// void drawing_Background(GContext *ctx){
//     // for convinience, change background here
// #ifdef PBL_COLOR
//     if (drawing_BT_State != 0 && drawing_Stance != 14){
//         graphics_context_set_fill_color(ctx, (drawing_Stance == 1) ? GColorVividCerulean : GColorOrange);
//         graphics_fill_rect(ctx, GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), 0, GCornerNone);
//     }
// #else
//     drawing_ChangeBitmap(drawing_Stance == 1);
//     graphics_draw_bitmap_in_rect(ctx, img_grid, GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
// #endif

//     // draw progress ring
//     if (main_StartTime > 0){
//         //int16_t percent = 96 - (uint32_t)(main_EndTime - getEpoch()) * 96 / (uint32_t)(main_EndTime - main_StartTime);
//         //drawing_BackColor(ctx, percent);
//         progressAngle = TRIG_MAX_ANGLE - TRIG_MAX_ANGLE * (main_EndTime - getEpoch()) / (main_EndTime - main_StartTime);
//     }
//     else if (main_StartTime == 0)
//         progressAngle = 0;
//     drawing_Progress(ctx, progressAngle);

//     // draw circle
// #ifdef PBL_COLOR
//     graphics_context_set_fill_color(ctx, (drawing_BT_State == 0 || drawing_Stance == 14) ? GColorMintGreen : ((drawing_Stance == 1) ? GColorCeleste : GColorPastelYellow));
// #else
//     graphics_context_set_fill_color(ctx, GColorWhite);
// #endif
//     graphics_fill_circle(ctx, GPoint(CIR_X, CIR_Y), CIR_R);

//     // draw controls
// #ifdef PBL_COLOR
//     graphics_context_set_fill_color(ctx, (drawing_BT_State == 0 || drawing_Stance == 14) ? GColorGreen : ((drawing_Stance == 1) ? GColorVividCerulean : GColorOrange));
// #else
//     graphics_context_set_fill_color(ctx, GColorBlack);
// #endif

//     // draw as path
//     GPathInfo BT_Path = {
//         .num_points = 4,
//         .points = BT_Left
//     };
//     GPath *path = gpath_create(&BT_Path);
//     gpath_draw_filled(ctx, path);
//     gpath_destroy(path);
//     BT_Path.points = BT_Right;
//     path = gpath_create(&BT_Path);
//     gpath_draw_filled(ctx, path);
//     gpath_destroy(path);

//     // make gray if on aplite
// #ifndef PBL_COLOR
//     graphics_context_set_compositing_mode(ctx, GCompOpOr);
//     graphics_draw_bitmap_in_rect(ctx, img_grid, GRect(CIR_X - CIR_R * 7 / 10, CIR_Y - CIR_R * 7 / 10, CIR_R * 14 / 10, CIR_R * 14 / 10));
//     graphics_context_set_compositing_mode(ctx, GCompOpSet);
// #endif

//     // draw "switch" halfway through side plankss
//     if (drawing_Stance == 13 && main_EndTime - getEpoch() < 15000 && main_EndTime - getEpoch() > 10000){
//         // draw text
//         graphics_context_set_text_color(ctx, GColorBlack);
//         graphics_draw_text(ctx, "Switch", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(0, CIR_Y + 35, SCREEN_WIDTH, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
//         // vibrate once
//         if (!onetimeVib) {
//             onetimeVib = true;
//             vibes_short_pulse();
//             drawing_Reversed = true;
//         }
//     }
//     else if (onetimeVib){
//         onetimeVib = false;
//     }
// }

// // set text display (and remove miroring of person)
// void drawing_SetText(TextLayer *text, uint8_t index){
//     text_layer_set_text(text, TXT_Current[index]);
//     drawing_Reversed = false;
// }

// // reset values
// void drawing_reset(void){
//     progressAngle = 0;
// }

// // create and destroy bitmap background
// void drawing_CreateBitmap(void){
// #ifdef PBL_SDK_3
//     img_grid = gbitmap_create_blank(GSize(4, 4), GBitmapFormat8Bit);
// #else
//     img_grid = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GRID_1);
// #endif
// }
// void drawing_DestroyBitmap(void){
//     gbitmap_destroy(img_grid);
// }
// uint8_t oldMode = 0;
// void drawing_ChangeBitmap(uint8_t mode){
//     // only change if different
//     if (oldMode == mode) return;
//     oldMode = mode;
//     // destroy old bitmap
//     gbitmap_destroy(img_grid);
//     // load new bitmap
//     if (mode == 0)
//         gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GRID_1);
//     else
//         gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GRID_2);
// }



// // ********** Animation ********** //
// // get interpolated distance
// static inline int16_t distance_interpolate(const int32_t normalized, int16_t from, int16_t to) {
//     return from + ((normalized * (to - from)) / ANIMATION_NORMALIZED_MAX);
// }
// static inline GPoint point_interpolate(const int32_t normalized, GPoint from, GPoint to) {
//     return GPoint(from.x + ((normalized * (to.x - from.x)) / ANIMATION_NORMALIZED_MAX), from.y + ((normalized * (to.y - from.y)) / ANIMATION_NORMALIZED_MAX));
// }
// // destroy animation on aplite
// static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
//   // free the animation
//   property_animation_destroy((PropertyAnimation*)animation);
// }
// // animation update
// void ani_update(Animation *ani, const uint32_t distance_normalized) {
//     // set values
//     person.head = point_interpolate(distance_normalized, oldPerson.head, newPerson.head);
//     person.chest = point_interpolate(distance_normalized, oldPerson.chest, newPerson.chest);
//     person.r_elbo = point_interpolate(distance_normalized, oldPerson.r_elbo, newPerson.r_elbo);
//     person.r_hand = point_interpolate(distance_normalized, oldPerson.r_hand, newPerson.r_hand);
//     person.l_elbo = point_interpolate(distance_normalized, oldPerson.l_elbo, newPerson.l_elbo);
//     person.l_hand = point_interpolate(distance_normalized, oldPerson.l_hand, newPerson.l_hand);
//     person.waist = point_interpolate(distance_normalized, oldPerson.waist, newPerson.waist);
//     person.r_knee = point_interpolate(distance_normalized, oldPerson.r_knee, newPerson.r_knee);
//     person.r_foot = point_interpolate(distance_normalized, oldPerson.r_foot, newPerson.r_foot);
//     person.l_knee = point_interpolate(distance_normalized, oldPerson.l_knee, newPerson.l_knee);
//     person.l_foot = point_interpolate(distance_normalized, oldPerson.l_foot, newPerson.l_foot);

//     // refresh
//     main_mark_dirty();
// }
// static const AnimationImplementation ani_implementation = {
//     .update = (AnimationUpdateImplementation)ani_update,
// };
// static void ani_stopped(Animation *animation, bool finished, void *context) {
// #ifndef PBL_SDK_3
//     // free the animation
//     property_animation_destroy((PropertyAnimation*)animation);
// #endif
//     // schedule next
//     ani_next_pose();
// }
// static void ani_next_pose(void){
//     // index pose
//     if (++drawing_Index >= Pose_Count[drawing_Stance]) drawing_Index = 0;

//     // change pose
//     oldPerson = person;
//     newPerson = data2screen(Pose_List[drawing_Stance][drawing_Index]);

//     // schedule animation
//     Animation *ani = animation_create();
//     animation_set_duration(ani, newPerson.duration);
//     animation_set_curve(ani, newPerson.curve);
//     animation_set_delay(ani, newPerson.delay);
//     animation_set_handlers(ani, (AnimationHandlers) { .stopped = ani_stopped }, NULL);
//     animation_set_implementation(ani, &ani_implementation);
//     animation_schedule(ani);
// }

// // button animation
// void ani_button_update(Animation *ani, const uint32_t distance_normalized) {
//     // set values
//     for (uint8_t ii = 0; ii < 4; ii++){
//         BT_Left[ii] = point_interpolate(distance_normalized, (drawing_BT_State > 0) ? BT_Play[ii] : BT_Pause_L[ii], (drawing_BT_State == 0) ? BT_Play[ii] : BT_Pause_L[ii]);
//         BT_Right[ii] = point_interpolate(distance_normalized, (drawing_BT_State > 0) ? BT_Play[ii] : BT_Pause_R[ii], (drawing_BT_State == 0) ? BT_Play[ii] : BT_Pause_R[ii]);      
//     }
//     // refresh
//     main_mark_dirty();
// }
// static const AnimationImplementation ani_button_implementation = {
//     .update = (AnimationUpdateImplementation)ani_button_update,
// };

// // create arbitrary animation
// static void drawing_create_animation(const AnimationImplementation *implement){
//     // schedule animation
//     Animation *ani = animation_create();
//     animation_set_duration(ani, 300);
//     animation_set_curve(ani, AnimationCurveEaseOut);
//     animation_set_implementation(ani, implement);
// #ifndef PBL_SDK_3
//     animation_set_handlers(ani, (AnimationHandlers) {
//         .stopped = anim_stopped_handler
//     }, NULL);
// #endif
//     animation_schedule(ani);
// }

// // change button state
// void drawing_toggle_button_mode(void){
//     // change mode
//     if (++drawing_BT_State > 1) drawing_BT_State = 0;
//     // animate
//     drawing_create_animation(&ani_button_implementation);
// }



// // ********** Person ********** //
// // draw thick lines
// void drawing_ThickLine(GContext *ctx, GPoint p0, GPoint p1){
// #ifdef PBL_SDK_3
//     graphics_draw_line(ctx, p0, p1);
// #else
//     graphics_draw_line(ctx, p0, p1);
//     graphics_draw_line(ctx, GPoint(p0.x + 1, p0.y), GPoint(p1.x + 1, p1.y));
//     graphics_draw_line(ctx, GPoint(p0.x - 1, p0.y), GPoint(p1.x - 1, p1.y));
//     graphics_draw_line(ctx, GPoint(p0.x, p0.y + 1), GPoint(p1.x, p1.y + 1));
//     graphics_draw_line(ctx, GPoint(p0.x, p0.y - 1), GPoint(p1.x, p1.y - 1));
//     graphics_draw_line(ctx, GPoint(p0.x + 1, p0.y + 1), GPoint(p1.x + 1, p1.y + 1));
//     graphics_draw_line(ctx, GPoint(p0.x - 1, p0.y + 1), GPoint(p1.x - 1, p1.y + 1));
//     graphics_draw_line(ctx, GPoint(p0.x + 1, p0.y - 1), GPoint(p1.x + 1, p1.y - 1));
//     graphics_draw_line(ctx, GPoint(p0.x - 1, p0.y - 1), GPoint(p1.x - 1, p1.y - 1));
//     graphics_draw_line(ctx, GPoint(p0.x + 2, p0.y), GPoint(p1.x + 2, p1.y));
//     graphics_draw_line(ctx, GPoint(p0.x - 2, p0.y), GPoint(p1.x - 2, p1.y));
//     graphics_draw_line(ctx, GPoint(p0.x, p0.y + 2), GPoint(p1.x, p1.y + 2));
//     graphics_draw_line(ctx, GPoint(p0.x, p0.y - 2), GPoint(p1.x, p1.y - 2));
// #endif
// }
// // set body poses
// struct HumanBody data2screen(struct HumanBody body){
//     // create new
//     struct HumanBody nBody;
//     // reverse
//     int16_t rev = (drawing_Reversed) ? -1 : 1;
//     // convert values
//     nBody.head = GPoint(CIR_X + body.head.x * rev, CIR_Y + body.head.y);
//     nBody.chest = GPoint(CIR_X + body.chest.x * rev, CIR_Y + body.chest.y);
//     nBody.r_elbo = GPoint(CIR_X + body.r_elbo.x * rev, CIR_Y + body.r_elbo.y);
//     nBody.r_hand = GPoint(CIR_X + body.r_hand.x * rev, CIR_Y + body.r_hand.y);
//     nBody.l_elbo = GPoint(CIR_X + body.l_elbo.x * rev, CIR_Y + body.l_elbo.y);
//     nBody.l_hand = GPoint(CIR_X + body.l_hand.x * rev, CIR_Y + body.l_hand.y);
//     nBody.waist = GPoint(CIR_X + body.waist.x * rev, CIR_Y + body.waist.y);
//     nBody.r_knee = GPoint(CIR_X + body.r_knee.x * rev, CIR_Y + body.r_knee.y);
//     nBody.r_foot = GPoint(CIR_X + body.r_foot.x * rev, CIR_Y + body.r_foot.y);
//     nBody.l_knee = GPoint(CIR_X + body.l_knee.x * rev, CIR_Y + body.l_knee.y);
//     nBody.l_foot = GPoint(CIR_X + body.l_foot.x * rev, CIR_Y + body.l_foot.y);
//     // timing
//     nBody.duration = body.duration;
//     nBody.delay = body.delay;
//     nBody.curve = body.curve;
//     // return
//     return nBody;
// }

// // set mode
// void drawing_set_mode(uint8_t mode, uint16_t duration){
//     // change pose
//     drawing_Index = 0;
//     drawing_Stance = mode;
//     // update
//     oldPerson = person;
//     newPerson = data2screen(Pose_List[drawing_Stance][drawing_Index]);

//     // animate
//     if (duration == 0)
//         person = newPerson;
//     else{
//         Animation *ani = animation_create();
//         animation_set_duration(ani, newPerson.duration);
//         animation_set_curve(ani, AnimationCurveEaseInOut);
//         animation_set_delay(ani, newPerson.delay);
//         animation_set_handlers(ani, (AnimationHandlers) { .stopped = ani_stopped }, NULL);
//         animation_set_implementation(ani, &ani_implementation);
//         animation_schedule(ani);
//     }
// }
// // draw person
// void drawing_Person(GContext *ctx){
//     // set up drawing
//     graphics_context_set_stroke_color(ctx, GColorBlack);
// #ifdef PBL_SDK_3
//     graphics_context_set_stroke_width(ctx, 4);
// #endif

//     // draw body
//     drawing_ThickLine(ctx, person.head, person.chest);
//     drawing_ThickLine(ctx, person.chest, person.waist);
//     // draw head
//     graphics_context_set_fill_color(ctx, GColorBlack);
//     graphics_fill_circle(ctx, person.head, 16);
//     graphics_context_set_fill_color(ctx, GColorWhite);
//     graphics_fill_circle(ctx, person.head, 12);
//     // draw right arm
//     drawing_ThickLine(ctx, person.chest, person.r_elbo);
//     drawing_ThickLine(ctx, person.r_elbo, person.r_hand);
//     // draw left arm
//     drawing_ThickLine(ctx, person.chest, person.l_elbo);
//     drawing_ThickLine(ctx, person.l_elbo, person.l_hand);
//     // draw right leg
//     drawing_ThickLine(ctx, person.waist, person.r_knee);
//     drawing_ThickLine(ctx, person.r_knee, person.r_foot);
//     // draw left leg
//     drawing_ThickLine(ctx, person.waist, person.l_knee);
//     drawing_ThickLine(ctx, person.l_knee, person.l_foot);
// }
// // draw props with person
// void drawing_Props(GContext *ctx){
//     // set up drawing
//     graphics_context_set_stroke_color(ctx, GColorBlack);
// #ifdef PBL_SDK_3
//     graphics_context_set_stroke_width(ctx, 4);
// #endif

//     // check mode
//     switch (drawing_Stance){
//         case 3: // wall sit
//             drawing_ThickLine(ctx, GPoint(CIR_X - 12, CIR_Y - 44), GPoint(CIR_X - 12, CIR_Y + 41));
//         break;
//         case 6: // chair step
//             drawing_ThickLine(ctx, GPoint(CIR_X + 3, CIR_Y + 51), GPoint(CIR_X + 3, CIR_Y + 36));
//             drawing_ThickLine(ctx, GPoint(CIR_X + 18, CIR_Y + 36), GPoint(CIR_X + 3, CIR_Y + 36));
//             drawing_ThickLine(ctx, GPoint(CIR_X + 18, CIR_Y + 51), GPoint(CIR_X + 18, CIR_Y + 36));
//             drawing_ThickLine(ctx, GPoint(CIR_X + 22, CIR_Y + 17), GPoint(CIR_X + 18, CIR_Y + 36));
//         break;
//         case 8: // chair dip
//             drawing_ThickLine(ctx, GPoint(CIR_X + 4, CIR_Y + 37), GPoint(CIR_X + 4, CIR_Y + 22));
//             drawing_ThickLine(ctx, GPoint(CIR_X + 19, CIR_Y + 22), GPoint(CIR_X + 4, CIR_Y + 22));
//             drawing_ThickLine(ctx, GPoint(CIR_X + 19, CIR_Y + 37), GPoint(CIR_X + 19, CIR_Y + 22));
//             drawing_ThickLine(ctx, GPoint(CIR_X + 23, CIR_Y + 3), GPoint(CIR_X + 19, CIR_Y + 22));
//         break;
//     }
// }