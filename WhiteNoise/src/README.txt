The main file here is the white noise Cg shader md5.cg. It is very easy to use: just bind it as the pixel shader, and draw a screen-aligned rectangle with texture coordinates specified at the 4 corners. It should be directly applicable for vertex shader as well.

The entry routine is fragment() which calls UniformRandom01() as in the bottom of the file. There are 2 uniform parameters: key, which lets you "seed" the output noise (i.e. the noise is completely determined by the key and the texture coordinates), and res, for window size. The latter can be rid of if you are using unnormalized texture coordinates.

The code is based on MD5, a cryptographic hash, so the quality is impeccable (for random noise) but it can be too slow for some applications. Right now, it runs at about 16M samples per second on a G80 chip. Further speedups can be achieved by skipping some of the scrambling rounds in routine MD5unroll(), even with only the 16 FF rounds the quality is still better than rand() or drand48().

You can find more information (paper, video, etc) at http://research.microsoft.com/research/pubs/view.aspx?tr_id=1384

