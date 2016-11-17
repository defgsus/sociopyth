
def plot_image(img, xy, color="hsl(0,0%,0%)", xy_ref=None):
    """Plot the list of x,y value pairs"""
    from PIL import Image, ImageDraw
    if not xy:
        return
    # normalize coords
    width = img.size[0]
    height = img.size[1]
    if not xy_ref:
        xy_ref = xy
    mi_x = ma_x = xy_ref[0][0]
    mi_y = ma_y = xy_ref[0][1]
    for p in xy_ref:
        mi_x = min(mi_x, p[0])
        ma_x = max(ma_x, p[0])
        mi_y = min(mi_y, p[1])
        ma_y = max(ma_y, p[1])
    scale_x = width
    scale_y = height
    if ma_x > mi_x:
        scale_x /= (ma_x - mi_x)
    if (ma_y > mi_y):
        scale_y /= (ma_y - mi_y)
    coords = []
    for p in xy:
        coords.append(((p[0]-mi_x)*scale_x, height-1-(p[1]-mi_y)*scale_y,))
    draw = ImageDraw.Draw(img)
    print coords
    draw.line(coords, fill=color, width=2)



def get_plot_image(xy, width=320,height=200,color="hsl(0,0%,0%)",xy_ref=None):
    """Plot the list of x,y value pairs"""
    from PIL import Image
    img = Image.new("RGB", (width, height), "white")
    plot_image(img, xy, color=color, xy_ref=xy_ref)
    return img



if __name__ == "__main__":
    img = get_plot_image([(0,0),(1,2),(2,1.3),(3,0),(4,1)])
    img.show()