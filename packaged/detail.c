#include "stdBoggle.h"
#include "soln.h"

extern char bd[4][4];
extern int prev[4][4];

static void solve(int x, int y, int n, char* wdSoFar)
{
	int cx, cy;
	//printf("%d: %s\n", n, wdSoFar);
	
	for (int dx=-1; dx<=1; dx++) {
		for (int dy=-1; dy<=1; dy++) {
			if (dx || dy) {
				cx = x + dx;
				cy = y + dy;
				
				if (cx >= 0 && cx < 4 &&
					cy >= 0 && cy < 4 &&
					!prev[cx][cy])
				{
					wdSoFar[n] = bd[cx][cy];
					if (wdSoFar[n] == 'q')
						wdSoFar[n++ + 1] = 'u';
					wdSoFar[n+1] = '\0';
					
					//printf("%s\n", wdSoFar);
					if (startsWord(wdSoFar)) {
						//printf(">%s\n",wdSoFar);
						if (n>=2 && isWord(wdSoFar)) {
							addSoln(wdSoFar, cx, cy); //addItem(wdSoFar, &soln);
						}
						
						prev[cx][cy] = 1+n;
						solve(cx, cy, n+1, wdSoFar);
						prev[cx][cy] = 0;
					}
					
					if (wdSoFar[n-1] == 'q')
						n--;
				}
				
			}
		}
	}
}

void doDetailedSearch()
{
	char wd[17] = "";
	int n = 0;
	
	initSoln();
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			prev[i][j] = 0;
	
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			wd[0] = bd[i][j];
			n = 1;
			if (wd[0] == 'q') {
				wd[1] = 'u';
				n=2;
			}
			wd[n] = '\0';
			prev[i][j] = n;
			solve(i, j, n, wd);
			prev[i][j] = 0;
		}
	}
	
	printSolns();
}
