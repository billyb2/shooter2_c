pub struct AxisAlignedBoundingBox {
    pub pos_x: f32,
    pub pos_y: f32,
    pub width: f32,
    pub height: f32,

}

pub trait AsAABB {
    fn as_aabb(&self) -> AxisAlignedBoundingBox;
}


pub fn aabb_collision(aabb1: &dyn AsAABB, aabb2: &dyn AsAABB) -> bool {
    let obj1 = aabb1.as_aabb();
    let obj2 = aabb2.as_aabb();

    obj1.pos_x - obj1.width <= obj2.pos_x + obj2.width &&
    obj1.pos_x + obj1.width >= obj2.pos_x &&
    obj1.pos_y - obj1.height <= obj2.pos_y + obj2.height &&
    obj1.pos_y + obj2.height >= obj2.pos_y

}
