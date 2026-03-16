import cypari2
import matplotlib.pyplot as plt
import matplotlib
from matplotlib.gridspec import GridSpec
from matplotlib.patheffects import withStroke
from matplotlib.patches import Polygon
import numpy as np

matplotlib.use('TkAgg')

CMAP = plt.cm.coolwarm
COLOR_UNBOUNDED = CMAP(0.05)
COLOR_OVAL = CMAP(0.95)
MARKER_COLOR = 'orange'

def z_to_torus(t_complex, w1, w2, R, r):
    M = np.array([[w1.real, w2.real],
                  [w1.imag, w2.imag]])
    rhs = np.array([t_complex.real, t_complex.imag])
    su = np.linalg.solve(M, rhs)
    phi = 2*np.pi*su[0]
    theta = 2*np.pi*su[1]
    X = (R + r * np.cos(theta))*np.cos(phi)
    Y = (R + r * np.cos(theta))*np.sin(phi)
    Z = r*np.sin(theta)
    return X, Y, Z

def plotTorus(R, r, ax, discriminant, precision=100):
    U = np.linspace(0, 2*np.pi, precision)
    V = np.linspace(0, 2*np.pi, precision)
    U, V = np.meshgrid(U, V)
    X = (R + r * np.cos(V))*np.cos(U)
    Y = (R + r * np.cos(V))*np.sin(U)
    Z = r*np.sin(V)

    # u_coord = (U / (2*np.pi)) % 1.0
    v_coord = (V / (2*np.pi)) % 1.0

    if discriminant > 0:
        scalar = np.where(v_coord < 0.5, 0.05, 0.95)
    else:
        scalar = np.full_like(v_coord, 0.05)

    facecolors = CMAP(scalar)

    ax.plot_surface(X, Y, Z, facecolors=facecolors, antialiased=True, alpha=0.6,
                             shade=True, lightsource=matplotlib.colors.LightSource(azdeg=225, altdeg=45))
    return ax

def draw_fundamental_domain(ax, w1, w2):
    corners = np.array([
        [0, 0],
        [w1.real, w1.imag],
        [w1.real + w2.real, w1.imag + w2.imag],
        [w2.real, w2.imag]
    ])
    patch = Polygon(corners, closed=True, facecolor='steelblue', alpha=0.15, edgecolor='navy')
    ax.add_patch(patch)
    ax.set_xlim(corners[:, 0].min() - 0.1, corners[:, 0].max() + 0.1)
    ax.set_ylim(corners[:, 1].min() - 0.1, corners[:, 1].max() + 0.1)
    ax.set_aspect('equal')
    ax.set_xlabel('Re(z)')
    ax.set_ylabel('Im(z)')
    ax.set_title('Fundamental domain ℂ/Λ')

def plot_path_with_breaks(ax, path, color, lw=1.8):
    verts = path.vertices
    codes = path.codes
    xs = verts[:, 0].astype(float).copy()
    ys = verts[:, 1].astype(float).copy()
    if codes is not None:
        # Wherever there's a MOVETO (code 1) that isn't the very first point,
        # insert a NaN to lift the pen
        mask = (codes == 1)
        mask[0] = False
        xs[mask] = np.nan
        ys[mask] = np.nan
    ax.plot(xs, ys, color=color, lw=lw)

def classify_point(z, w2):
    # Return which component z belongs to: 'unbounded' or 'oval'.
    # Based on the normalised v-coordinate of z in the fundamental domain.
    v_coord = (z.imag / w2.imag) % 1.0
    return 'oval' if v_coord >= 0.5 else 'unbounded'

a = float(input("a: ") or -7)
b = float(input("b: ") or 6)
discriminant = -16*( 4*pow(a,3) + 27*pow(b, 2) )
pari = cypari2.Pari()
E = pari.ellinit([0, 0, 0, a, b])

g2 = -4*a
g3 = -4*b

periods = pari.ellperiods(E, 0)
w1p = float(pari(periods[0]).real()) + 1j * float(pari(periods[0]).imag())
w2p = float(pari(periods[1]).real()) + 1j * float(pari(periods[1]).imag())
if w2p.imag < 0:
    w2p = -w2p

if discriminant < 0:
    w1p = w1p.real + 0j
    w2p = w2p.imag * 1j

R1p = float(pari.abs(w1p)/(2*np.pi))
R2p = float(pari.abs(w2p)/(2*np.pi))


fig = plt.figure(figsize=(18,8))
gs = GridSpec(1, 3, figure=fig)

ell_ax = fig.add_subplot(gs[0, 0])
fd_ax = fig.add_subplot(gs[0, 1])
ax3d = fig.add_subplot(gs[0, 2], projection='3d')

C = 20
roots = np.roots([1, 0, a, b])
real_roots = sorted(r.real for r in roots if abs(r.imag) < 1e-8)
x_min_curve = real_roots[0] if real_roots else -C
x_lo = min(-C, x_min_curve - 1)
x_hi = C

y_grid, x_grid = np.ogrid[x_lo:x_hi:300j, x_lo:x_hi:300j]
contour_set = ell_ax.contour(x_grid.ravel(), y_grid.ravel(), pow(y_grid, 2) - pow(x_grid, 3) - x_grid*a -b, [0], colors=['none'])

draw_fundamental_domain(fd_ax, w1p, w2p)

paths = contour_set.get_paths()

def path_bbox_area(p):
    verts = p.vertices
    return (verts[:, 0].ptp())*(verts[:, 1].ptp())

if len(paths) == 2:
    areas = [path_bbox_area(p) for p in paths]
    oval_idx = int(np.argmin(areas))
    unbounded_idx = 1- oval_idx
    component_colors = {oval_idx: COLOR_OVAL, unbounded_idx: COLOR_UNBOUNDED}
    for idx, path in enumerate(paths):
        plot_path_with_breaks(ell_ax, path, component_colors[idx])


    verts_oval = paths[oval_idx].vertices
    verts_unbounded = paths[unbounded_idx].vertices
    all_verts_list = [(verts_unbounded, 'unbounded'), (verts_oval, 'oval')]
else:
    plot_path_with_breaks(ell_ax, paths[0], COLOR_UNBOUNDED)
    all_verts_list = [(paths[0].vertices, 'unbounded')]

all_verts = np.vstack([vl[0] for vl in all_verts_list])

ax3d = plotTorus(max(R1p, R2p), min(R1p, R2p), ax3d, discriminant)

lim = 0.75
ax3d.set_xlim(-lim, lim)
ax3d.set_ylim(-lim, lim)
ax3d.set_zlim(-lim, lim)
ax3d.set_box_aspect((1, 1, 1))
ax3d.set_title("Torus ℂ/Λ")



eq_text = r"$y^2 = x^3 + ({})x + {}$".format(a, b)
disc_text = r"$\Delta = {}$".format(discriminant)
full_label = f"{eq_text}\n{disc_text}"

plt.figtext(0.65, 0.02, full_label, ha='left', fontsize=11, bbox={"facecolor":"wheat", "alpha":0.5, "pad":5})
ell_ax.set_title("Elliptic curve")
ell_ax.grid(True)
ell_ax.set_xlim(x_lo, x_hi)

if len(paths) == 2:
    from matplotlib.lines import Line2D
    legend_elements = [
        Line2D([0], [0], color=COLOR_UNBOUNDED, lw=2, label='Unbounded branch'),
        Line2D([0], [0], color=COLOR_OVAL, lw=2, label='Oval'),
    ]
    ell_ax.legend(handles=legend_elements, loc='upper left', fontsize=8)

def snap_to_curve(mx, my):
    dists = (all_verts[:, 0] - mx)**2 + (all_verts[:, 1] - my)**2
    return float(all_verts[np.argmin(dists), 0]), float(all_verts[np.argmin(dists), 1])

def point_component(px, py):
    if len(all_verts_list) == 1:
        return 'unbounded'
    verts_u = all_verts_list[0][0]
    dists_u = (verts_u[:, 0] - px)**2 + (verts_u[:, 1] - py)**2
    verts_o = all_verts_list[1][0]
    dists_o = (verts_o[:, 0] - px)**2 + (verts_o[:, 1] - py)**2
    return 'unbounded' if dists_u.min() < dists_o.min() else 'oval'

_m2d = ell_ax.scatter([], [], color=COLOR_UNBOUNDED, edgecolors='white', linewidths=1.5, s=100, zorder=10)
_m3d_ref = [None]
_mfd = fd_ax.scatter([], [], color=COLOR_UNBOUNDED, edgecolors='white', linewidths=1.5, s=100, zorder=10)
_ann = ell_ax.text(0.03, 0.04, '', transform=ell_ax.transAxes, fontsize=9, bbox=dict(facecolor='white', edgecolor='gray', alpha=0.85, pad=4))
_ann_fd = fd_ax.text(0.03, 0.04, '', transform=fd_ax.transAxes, fontsize=9, bbox=dict(facecolor='white', edgecolor='gray', alpha=0.85, pad=4))

state = dict(dragging=False)

def _torus_and_fd(z, w1, w2):
    M = np.array([[w1.real, w2.real], [w1.imag, w2.imag]])
    su = np.linalg.solve(M, np.array([z.real, z.imag])) % 1.0
    z_r = su[0]*w1 + su[1]*w2

    X, Y, Z = z_to_torus(z, w1, w2, max(R1p, R2p), min(R1p, R2p))
    comp = classify_point(z, w2)
    marker_color = COLOR_OVAL if comp == 'oval' else COLOR_UNBOUNDED


    if _m3d_ref[0] is not None:
        _m3d_ref[0].remove()

    azim = np.radians(ax3d.azim)
    elev = np.radians(ax3d.elev)
    cam = np.array([np.cos(elev)*np.cos(azim), np.cos(elev)*np.sin(azim), np.sin(elev)])
    raw_depth = float(np.dot([X,Y, Z], cam))
    R_max = max(R1p, R2p) + min(R1p, R2p)
    alpha = float(np.clip(0.1 + 0.8*(raw_depth/R_max + 1) / 2, 0.1, 1.0))
    base = np.array(matplotlib.colors.to_rgb(marker_color))
    rgba = (*base, alpha)

    _m3d_ref[0] = ax3d.text(X, Y, Z, u'●', color=rgba, fontsize=14, ha='center', va='center',
                            path_effects=[
                                withStroke(linewidth=2.5, foreground=(1.0, 1.0, 1.0, max(0.2, alpha*0.8)))
                            ])

    _mfd.set_offsets([[z_r.real, z_r.imag]])
    _mfd.set_facecolor(marker_color)

    return z_r, marker_color

def update_from_curve(mx, my, w1, w2):
    px, py = snap_to_curve(mx, my)
    z_pari = pari.ellpointtoz(E, [px, py])
    z = complex(float(pari(z_pari).real()), float(pari(z_pari).imag()))
    z_r, col = _torus_and_fd(z, w1, w2)

    _m2d.set_offsets([[px, py]])
    _m2d.set_facecolor(col)
    _ann.set_text(f"$(x,y)=({px:.3f},\\ {py:.3f})$\n$z={z_r.real:.4f}{z_r.imag:+.4f}i$")
    _ann_fd.set_text('')

    fig.canvas.draw_idle()

def update_from_fd(mx, my, w1, w2):
    z = complex(mx, my)
    z_r, col = _torus_and_fd(z, w1, w2)
    _ann_fd.set_text(f"$z={z_r.real:.4f}{z_r.imag:+.4f}i$")
    fig.canvas.draw_idle()

def on_press1(event):
    if event.xdata is None:
        return
    if event.inaxes is ell_ax:
        state['dragging'] = True
        update_from_curve(event.xdata, event.ydata, w1p, w2p)
    elif event.inaxes is fd_ax:
        state['dragging'] = True
        update_from_fd(event.xdata, event.ydata, w1p, w2p)

def on_motion1(event):
    if not state['dragging'] or event.xdata is None:
        return
    if event.inaxes is ell_ax:
        update_from_curve(event.xdata, event.ydata, w1p, w2p)
    elif event.inaxes is fd_ax:
        update_from_fd(event.xdata, event.ydata, w1p, w2p)

def on_release(event):
    state['dragging'] = False



fig.canvas.mpl_connect('button_press_event', on_press1)
fig.canvas.mpl_connect('motion_notify_event', on_motion1)
fig.canvas.mpl_connect('button_release_event', on_release)

plt.show()