#pragma scop
/* Clay
   fuse([0, 0]);
*/
for (i = 0; i <= N; i++) {
  for (j = 0; j <= M; j++) {
    a[j] = 0;
  }
  for (j = 0; j <= M; j++) {
    t[j] = 0;
    s[j] = 0;
  }
}
#pragma endscop
