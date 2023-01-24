#include <cstdio>
#include <cmath>
#include <algorithm>

const int N=505,M=5e5+5;
int n,m,tot,lnk[N],ter[M],nxt[M],st[M],ed[M],deg[N];
double a[N][N],b[N],x[N],f[M];

void add(int u,int v) {
	ter[++tot]=v,nxt[tot]=lnk[u],lnk[u]=tot;
}
void Gauss(int n) {
	for(int i=1;i<=n;++i) {
		int p=i;
		for(int k=i+1;k<=n;++k) if(fabs(a[k][i])>fabs(a[p][i])) p=k;
		if(i!=p) std::swap(a[i],a[p]),std::swap(b[i],b[p]);
		for(int k=i+1;k<=n;++k) {
			double d=a[k][i]/a[i][i];
			b[k]-=d*b[i];
			for(int j=i;j<=n;++j) a[k][j]-=d*a[i][j];
		}
	}
	for(int i=n;i>=1;--i) {
		for(int j=i+1;j<=n;++j) b[i]-=x[j]*a[i][j];
		x[i]=b[i]/a[i][i];
	}
}
int main() {
	scanf("%d%d",&n,&m);
	for(int i=1;i<=m;++i) {
		scanf("%d%d",&st[i],&ed[i]);
		add(st[i],ed[i]),add(ed[i],st[i]);
		++deg[st[i]],++deg[ed[i]];
	}
	for(int u=1;u<n;++u) {
		a[u][u]=1.0;
		for(int i=lnk[u];i;i=nxt[i]) {
			int v=ter[i];
			if(v!=n) a[u][v]=-1.0/deg[v];
		}
	}
	b[1]=1;
	Gauss(n-1);
	for(int i=1;i<=m;++i) {
		int a=st[i],b=ed[i];
		if(a!=n) f[i]+=x[a]/deg[a];
		if(b!=n) f[i]+=x[b]/deg[b];
	}
	std::sort(f+1,f+m+1);
	double ans=0;
	for(int i=1;i<=m;++i) ans+=(m-i+1)*f[i];
	printf("%.3lf\n",ans);
}