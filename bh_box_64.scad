t = 2;
inner = 192;
outer = inner + 2*t;
frame = 14; // thickness of the led frame
relief = 7; // distance from the back of the led frame to the back of the box
glass_t = 3;
stop_pos = frame + 20; //distance of the back of the display to the front of the glass 
box_depth = stop_pos + relief; // full depth of the box

module sides () {difference (){
    cube([outer,outer,box_depth],center = true);
    cube([inner, inner,box_depth*2],center = true); // full hole
}
}

module sides_front_back() {
    difference() {
    sides();
    translate([0,0,box_depth/2-glass_t/2]) cube([inner+t,inner+t,glass_t],center=true);
//    translate([-outer/2+t/2,0,0]) cube([t,inner+t,inner+t],center=true);
}
}

module stop() {
    cube([4,4,5],center = true);
}


module box() {union() {
    sides_front_back();
    translate([0,inner/2,box_depth/2-2.5-stop_pos]) stop();
    translate([inner/2,0,box_depth/2-2.5-stop_pos]) stop();
    translate([0,-inner/2,box_depth/2-2.5-stop_pos]) stop();
    translate([-inner/2,0,box_depth/2-2.5-stop_pos]) stop();
//    translate([outer/2-2-stop_pos,inner/2,-inner/4]) stop();
//    translate([outer/2-2-stop_pos,-inner/2,inner/4]) stop();
//    translate([outer/2-2-stop_pos,-inner/2,-inner/4]) stop();
}}

 //  base
module base(){
translate([-outer/2 -0+t/2,0,0]) cube([t,inner+t-0.2,inner+t-0.2],center=true);
}
//sides();
box();
//base();