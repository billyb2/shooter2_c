use std::f32::consts::{PI, FRAC_PI_2};
use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncWriteExt, AsyncReadExt, self};
use tokio::sync::RwLock;
use std::sync::Mutex;
use std::sync::Arc;
use once_cell::sync::Lazy;
use macroquad::prelude::*;

struct GameData {
    shapes: Vec<Drawable>,
    direction: Direction,

}

static GAME_DATA: Lazy<Arc<Mutex<GameData>>> = Lazy::new(|| Arc::new(Mutex::new( GameData { shapes: Vec::new(), direction: Direction::None })));
static MOUSE_POS: Lazy<Arc<Mutex<(f32, f32)>>> = Lazy::new(|| Arc::new(Mutex::new((0.0, 0.0))));


enum Shape {
    Circle,
    Rectangle,

}

struct Drawable {
    pos_x: f32,
    pos_y: f32,
    size_x: f32,
    size_y: f32, 
    shape: Shape,

}

impl Drawable {
    const fn zero() -> Self {
        Self {
            pos_x: 0.0,
            pos_y: 0.0,
            size_x: 0.0,
            size_y: 0.0,
            shape: Shape::Circle,

        }

    }

}

#[repr(u8)]
#[derive(Copy, Clone)]
enum Direction {
    None = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4,

}

#[macroquad::main("hello")]
async fn main() {
    let tokio_rt = tokio::runtime::Runtime::new().unwrap();

    tokio_rt.spawn(async move {
        let listener = TcpListener::bind("0.0.0.0:42069").await.unwrap();


        let stream = Arc::new(tokio::sync::Mutex::new(listener.accept().await.unwrap().0));

        tokio::task::spawn(handle_messages(stream.clone()));
        tokio::task::spawn(send_input(stream.clone()));


    });


    loop {
        clear_background(WHITE);

        {
            let game_data = &mut GAME_DATA.lock().unwrap();

            {
                *MOUSE_POS.lock().unwrap() = mouse_position();

            };

            if is_key_down(KeyCode::W) {
                game_data.direction = Direction::Up;

            } else if is_key_down(KeyCode::S) {
                game_data.direction = Direction::Down;

            } else if is_key_down(KeyCode::A) {
                game_data.direction = Direction::Left;

            } else if is_key_down(KeyCode::D) {
                game_data.direction = Direction::Right;

            } else {
                game_data.direction = Direction::None;

            }

            for drawable in game_data.shapes.iter() {
                match drawable.shape {
                    Shape::Rectangle => draw_rectangle(drawable.pos_x * 1.25, drawable.pos_y * 1.25, drawable.size_x * 1.25, drawable.size_y * 1.25, RED),
                    Shape::Circle => draw_circle(drawable.pos_x * 1.25, drawable.pos_y * 1.25, drawable.size_x * 1.25, RED),

                };

            }
        }


        next_frame().await;

    }


}

async fn handle_messages(stream: Arc<tokio::sync::Mutex<TcpStream>>) {
    let mut msg_buffer = [0; 1024];

    loop {
        let buffer_len = stream.lock().await.try_read(&mut msg_buffer);
        println!("Read");

        if let Ok(buffer_len) = buffer_len {
            let buffer = &msg_buffer[..buffer_len];

            if buffer_len >= 9 {
                let num_players = buffer[0];
                let shapes = &mut GAME_DATA.lock().unwrap().shapes;
                shapes.clear();

                if num_players as usize > shapes.len() {
                    for _ in 0..(num_players as usize - shapes.len()){
                        shapes.push(Drawable::zero());

                    }

                }

                for (i, chunk) in buffer[1..1 + num_players as usize * 8].chunks(8).enumerate() {
                    shapes[i] = Drawable {
                        pos_x: f32::from_le_bytes(chunk[0..4].try_into().unwrap()),
                        pos_y: f32::from_le_bytes(chunk[4..8].try_into().unwrap()),
                        size_x: 10.0,
                        size_y: 10.0,
                        shape: Shape::Circle,

                    };

                }
                
                let projectiles_start: usize = num_players as usize * 8 + 1;
                let num_projectiles = u16::from_le_bytes(buffer[projectiles_start..=projectiles_start + 1].try_into().unwrap());

                if num_players as usize + num_projectiles as usize > shapes.len() {
                    for _ in 0..(num_projectiles as usize + num_players as usize - shapes.len()) {
                        shapes.push(Drawable::zero());
                        
                    }

                }

                for (i, chunk) in buffer[projectiles_start + 2..projectiles_start + 2 + num_projectiles as usize * 9].chunks(9).enumerate() {
                    shapes[num_players as usize + i] = Drawable {
                        pos_x: f32::from_le_bytes(chunk[0..4].try_into().unwrap()),
                        pos_y: f32::from_le_bytes(chunk[4..8].try_into().unwrap()),
                        size_x: chunk[8] as f32,
                        size_y: chunk[8] as f32,
                        shape: Shape::Circle,

                    };

                }

                msg_buffer = [0; 1024];

            }

        } else if let Some(err) = buffer_len.err() {
            match err.kind() {
                io::ErrorKind::WouldBlock => (),
                err => panic!("{err:?}"),
            };

        }

    }

}

async fn send_input(stream: Arc<tokio::sync::Mutex<TcpStream>>) {
    let mut buf = [0; 5];

    loop {
        /*
        if is_mouse_button_down(MouseButton::Left) {
            let shapes = &mut SHAPES.lock().unwrap();
            let mouse_pos = &mut MOUSE_POS.lock().unwrap();

            //buf[1..5].copy_from_slice(&get_angle(mouse_pos.0, mouse_pos.1, shapes[1].pos_x, shapes[1].pos_y).to_le_bytes());


        }
        */

        {
            
            let direction = {
                GAME_DATA.lock().unwrap().direction

            };


            buf[0] = direction as u8;

            if direction as u8 != Direction::None as u8 {
                stream.lock().await.write_all(&buf).await.unwrap();

            }

        };

    }


}

fn get_angle(cx: f32, cy: f32, ex: f32, ey: f32) -> f32 {
    //let dy = unsafe { fsub_fast(ey, cy) };
    //let dx = unsafe { fsub_fast(ex, cx) };
    let dy = ey - cy;
    let dx = ex - cx;

    let mut angle = match dx != 0.0 {
        // Returns the angle in radians
        true => (dy / dx).atan(),
        false => match dy > 0.0 {
            true => FRAC_PI_2,
            false => PI,
        },
    };

    angle = match cx < ex {
        true => angle - PI,
        false => angle,

    };

    angle
    
}
