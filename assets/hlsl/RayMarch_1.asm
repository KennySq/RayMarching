//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer Constants
// {
//
//   float4 Padding;                    // Offset:    0 Size:    16 [unused]
//   float EyeX;                        // Offset:   16 Size:     4
//   float EyeY;                        // Offset:   20 Size:     4
//   float EyeZ;                        // Offset:   24 Size:     4
//   float Fov;                         // Offset:   28 Size:     4
//   float2 UV;                         // Offset:   32 Size:     8 [unused]
//   float PosX;                        // Offset:   40 Size:     4
//   float PosY;                        // Offset:   44 Size:     4
//   float PosZ;                        // Offset:   48 Size:     4
//   float AnimateSpeed;                // Offset:   52 Size:     4
//   float Density;                     // Offset:   56 Size:     4
//   float AppTime;                     // Offset:   60 Size:     4
//   float Size;                        // Offset:   64 Size:     4
//   float Mask;                        // Offset:   68 Size:     4 [unused]
//   float Emission;                    // Offset:   72 Size:     4 [unused]
//   int MaxSteps;                      // Offset:   76 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// DefaultSamplerState               sampler      NA          NA             s0      1 
// mCloudTexture                     texture  unorm4          3d             t0      1 
// Constants                         cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0      POS   float       
// TEXCOORD                 0   xy          1     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
ps_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[5], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture3d (unorm,unorm,unorm,unorm) t0
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 6
dp3 r0.x, -cb0[1].xyzx, -cb0[1].xyzx
rsq r0.x, r0.x
mul r0.xyz, r0.xxxx, -cb0[1].xyzx
mul r1.xyz, r0.zxyz, l(1.000000, 0.000000, 0.000000, 0.000000)
mad r1.xyz, r0.yzxy, l(0.000000, 0.000000, 1.000000, 0.000000), -r1.xyzx
dp2 r0.w, r1.xzxx, r1.xzxx
rsq r0.w, r0.w
mul r1.xyz, r0.wwww, r1.xyzx
mul r2.xyz, r0.yzxy, r1.zxyz
mad r2.xyz, r1.yzxy, r0.zxyz, -r2.xyzx
mad r3.xy, v1.xyxx, l(1280.000000, 720.000000, 0.000000, 0.000000), cb0[1].xyxx
add r3.xy, r3.xyxx, l(-640.000000, -360.000000, 0.000000, 0.000000)
mul r0.w, cb0[1].w, l(0.017453)
sincos r4.x, r5.x, r0.w
div r0.w, r4.x, r5.x
div r0.w, l(720.000000), r0.w
mad r0.w, r0.w, l(0.500000), cb0[1].z
mov r3.z, -r0.w
dp3 r0.w, r3.xyzx, r3.xyzx
rsq r0.w, r0.w
mul r3.xyz, r0.wwww, r3.xyzx
mul r2.xyz, r2.xyzx, r3.yyyy
mad r1.xyz, r3.xxxx, r1.xyzx, r2.xyzx
mad r0.xyz, r3.zzzz, -r0.xyzx, r1.xyzx
itof r0.w, cb0[4].w
div r0.w, l(1.000000, 1.000000, 1.000000, 1.000000), r0.w
mov r1.xy, -cb0[2].zwzz
mov r1.z, -cb0[3].x
mov r1.w, l(0)
mov r2.xyz, l(0.010000,0,0,0)
loop 
  ige r2.w, r2.z, cb0[4].w
  breakc_nz r2.w
  mad r3.xyz, r2.xxxx, r0.xyzx, cb0[1].xyzx
  mul r2.w, r0.w, r3.z
  mad r4.xy, r3.xyxx, r0.wwww, l(0.500000, 0.500000, 0.000000, 0.000000)
  mad r4.z, cb0[3].w, cb0[3].y, r2.w
  sample_indexable(texture3d)(unorm,unorm,unorm,unorm) r2.w, r4.xyzx, t0.yzwx, s0
  log r2.w, r2.w
  mul r2.w, r2.w, cb0[3].z
  exp r2.w, r2.w
  add r4.xyz, r1.xyzx, r3.xyzx
  add r4.xyz, |r4.xyzx|, -cb0[4].xxxx
  max r3.w, r4.z, r4.y
  max r3.w, r3.w, r4.x
  min r3.w, r3.w, l(0.000000)
  max r4.xyz, r4.xyzx, l(0.000000, 0.000000, 0.000000, 0.000000)
  dp3 r4.x, r4.xyzx, r4.xyzx
  sqrt r4.x, r4.x
  add r3.w, r3.w, r4.x
  dp3 r3.x, r3.xyzx, r3.xyzx
  sqrt r3.x, r3.x
  add r3.x, r3.x, -cb0[4].x
  min r3.x, r3.x, r3.w
  add r2.y, r2.w, r2.y
  max r2.w, r2.w, r3.x
  add r2.x, r2.w, r2.x
  mul r2.w, r0.w, r2.y
  mul r2.w, r2.x, r2.w
  mul r2.w, r2.w, l(-1.442695)
  exp r1.w, r2.w
  iadd r2.z, r2.z, l(1)
endloop 
mov o0.xyzw, r1.wwww
ret 
// Approximately 65 instruction slots used