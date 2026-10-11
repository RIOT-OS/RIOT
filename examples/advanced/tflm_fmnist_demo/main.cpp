/*
 * SPDX-FileCopyrightText: 2025 Your Name Andrej Sum-Shik <andrej.sum-shik@stud.fra-uas.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       TFLite Micro Fashion-MNIST inference example.
 *
 * This example loads a quantized INT8 TensorFlow Lite Micro model and runs
 * inference on a set of Fashion-MNIST samples. It demonstrates how to
 * integrate TFLite Micro with RIOT and how to perform runtime quantization
 * of input data.
 *
 * @author      Andrej Sum-Shik <andrej.sum-shik@stud.fra-uas.de>
 */

 #include <stdio.h>
 #include <stdint.h>
 #include <string.h>
 #include <math.h>
 
 #include "ztimer.h"
 #include "samples.h"
 /* TFLite Micro */
 #include "tensorflow/lite/schema/schema_generated.h"
 #include "tensorflow/lite/micro/micro_interpreter.h"
 #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
 #include "tensorflow/lite/micro/system_setup.h"
 
 /* Your quantized model bytes from model_data.cc */
 extern "C" {
 extern const unsigned char g_model[];
 extern const unsigned int g_model_len;
 }
 
 /* Adjust at build time: make TENSOR_ARENA_SIZE=262144 */
 #ifndef TENSOR_ARENA_SIZE
 #   define TENSOR_ARENA_SIZE   (200 * 1024)
 #endif
 
 static uint8_t tensor_arena[TENSOR_ARENA_SIZE];
 
 static const char *kClassNames[10] = {
     "T-shirt/top", "Trouser", "Pullover", "Dress", "Coat",
     "Sandal", "Shirt", "Sneaker", "Bag", "Ankle boot"
 };
 
 static void quantize_into_input(tflite::MicroInterpreter *interp,
                                 const uint8_t *img_u8)
 {
     TfLiteTensor *in = interp->input(0);
     /* Expect INT8 input, NHWC [1, H, W, 1] */
     const float scale = in->params.scale;
     const int   zp    = in->params.zero_point;
 
     /* Our header stores a flattened 48x48 uint8 image (0..255); convert to int8 */
     for (int i = 0; i < IMG_W * IMG_H; ++i) {
         float real01 = img_u8[i] / 255.0f;               /* [0,1] */
         int q = (int)lrintf(real01 / scale + (float)zp); /* quantize */
 
         if (q > 127) {
             q = 127;
         }
         else if (q < -128) {
             q = -128;
         }
 
         in->data.int8[i] = (int8_t)q;
     }
 }
 
 extern "C" int main(void)
 {
     puts("\nRIOT TFLM Fashion-MNIST (INT8) — runtime quantization\n");
 
     const tflite::Model *model = tflite::GetModel(g_model);
     if (model->version() != TFLITE_SCHEMA_VERSION) {
         printf("Model schema %d != runtime %d\n",
                (int)model->version(), (int)TFLITE_SCHEMA_VERSION);
         return 1;
     }
 
     /* Minimal resolver for the student model (conv + depthwise + avgpool + FC) */
     tflite::MicroMutableOpResolver<10> resolver;
     resolver.AddConv2D();
     resolver.AddDepthwiseConv2D();
     resolver.AddAveragePool2D();
     resolver.AddFullyConnected();
     resolver.AddReshape();
     resolver.AddMean();
     resolver.AddQuantize();
     resolver.AddDequantize();
 
     tflite::MicroInterpreter interpreter(model, resolver,
                                          tensor_arena, TENSOR_ARENA_SIZE);
     if (interpreter.AllocateTensors() != kTfLiteOk) {
         puts("AllocateTensors() failed. Increase TENSOR_ARENA_SIZE.");
         return 1;
     }
 
     const TfLiteTensor *in  = interpreter.input(0);
     const TfLiteTensor *out = interpreter.output(0);
     printf("Input  type=%d shape=[%d,%d,%d,%d] q=(scale=%g zp=%ld)\n",
            (int)in->type,
            (int)in->dims->data[0], (int)in->dims->data[1],
            (int)in->dims->data[2], (int)in->dims->data[3],
            (double)in->params.scale, (long)in->params.zero_point);
     printf("Output type=%d shape=[%d,%d] q=(scale=%g zp=%ld)\n\n",
            (int)out->type,
            (int)out->dims->data[0], (int)out->dims->data[1],
            (double)out->params.scale, (long)out->params.zero_point);
 
     /* If header size != model size, warn (still works if 48x48 vs model is 48x48) */
     if ((int)in->dims->data[1] != IMG_H || (int)in->dims->data[2] != IMG_W) {
         printf("WARNING: header image size %dx%d != model expects %dx%d\n",
                IMG_W, IMG_H,
                (int)in->dims->data[2], (int)in->dims->data[1]);
     }
 
     for (int i = 0; i < SAMPLE_COUNT; ++i) {
         quantize_into_input(&interpreter, g_images[i]);
 
         uint32_t t0 = ztimer_now(ZTIMER_USEC);
         if (interpreter.Invoke() != kTfLiteOk) {
             printf("[#%d] Invoke failed\n", i);
             continue;
         }
         uint32_t dt = ztimer_now(ZTIMER_USEC) - t0;
 
         /* Argmax on int8 logits */
         const int8_t *logits = interpreter.output(0)->data.int8;
         int best_k = 0;
         int best_v = logits[0];
 
         for (int k = 1; k < 10; ++k) {
             if (logits[k] > best_v) {
                 best_v = logits[k];
                 best_k = k;
             }
         }
 
         const int lbl = (int)g_labels[i]; /* 0..9 or 255 if unknown */
         if (lbl <= 9) {
             printf("[#%d] pred=%s  (true=%s)  time=%lu us\n",
                    i, kClassNames[best_k], kClassNames[lbl],
                    (unsigned long)dt);
         }
         else {
             printf("[#%d] pred=%s  (true=unknown)  time=%lu us\n",
                    i, kClassNames[best_k], (unsigned long)dt);
         }
     }
 
     puts("\nDone.");
     return 0;
 }