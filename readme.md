# GAME SPACE INVADER

Nội dung cần báo cáo của môn đồ án:

> "Mô phỏng trò chơi Space Invader, sử dụng joystick để điều khiển.
> Yêu cầu:
>
> - Dùng hardware random generator để sinh quái ngẫu nhiên.
> - Có âm thanh phát ra bằng còi buzzer."

## GIỚI THIỆU

**Đề bài**: _Mô phỏng trò chơi Space Invader_

**Sản phẩm:**

1. Di chuyển 4 hướng bằng Joystick.
2. Tích luỹ điểm khi tiêu diệt quái, kết thúc khi trò chơi vượt quá giới hạn 3 mạng khi tàu bị bắn trúng hoặc va phải quái.
3. Tạo ra quái vị trí ngẫu nhiên.
4. Phát ra âm thanh và sáng led LD3 khi tiêu diệt quái.

- Ảnh chụp minh họa:\
  ![Ảnh minh họa](images/space_inavader.jpg)

## TÁC GIẢ

- Tên nhóm: VEmb
- Thành viên trong nhóm
  |STT|Họ tên|MSSV|Công việc|
  |--:|--|--|--|
  |1|Nguyễn Hiệp Hồng Quân|20215633|Tạo quái hiển thị ngẫu nhiên, kết nối joystick, kết nối còi buzeer và led LD3 khi tiêu diệt quái|
  |2|Nguyễn Thành Bách|20204812|Tạo giao diện game, xử lý logic tiêu diệt quái, điểm số, số mạng, điều khiển di chuyển bằng joystick|

## MÔI TRƯỜNG HOẠT ĐỘNG

- Sử dụng vi điều khiển STM32F429-DISC (cụ thể STM32F429ZIT6).
- Sử dụng joystick, còi buzzer.

## SƠ ĐỒ SCHEMATIC

_Cách nối dây, kết nối giữa các linh kiện_
|STM32F429|Joystick|Còi buzzer|
|--|--|--|
|3.3V|VCC||
|GND|GND|GND|
|PA1|VRx|
|PA2|VRy|
|PG13||VCC (led trên board)|

### TÍCH HỢP HỆ THỐNG

- Thành phần phần cứng:

| Thành phần | Vai trò                                                      |
| ---------- | ------------------------------------------------------------ |
| STM32F429  | Xử lý logic trò chơi, điều khiển ngoại vi, tạo số ngẫu nhiên |
| Joystick   | Điều khiển tàu (4 hướng)                                     |
| Còi buzzer | Tạo âm thanh khi tiêu diệt quái                              |

- Thành phần phần mềm:

| Thành phần | Vai trò                                                    |
| ---------- | ---------------------------------------------------------- |
| Firmware   | Điều khiển trò chơi, xử lý joystick, buzzer, màn hình, RNG |

### ĐẶC TẢ HÀM

- Giải thích một số hàm quan trọng: ý nghĩa của hàm, tham số vào, ra

  ```C
     // Hàm cập nhật vị trí của kẻ địch
    /**
      *  Hàm updateEnemy quản lý di chuyển và tạo kể địch mới, sử dụng RNG phần cứng để tạo ngẫu nhiên.

      *  Tham số @param dt: khoảng thời gian trôi qua, để cập nhật vị trí và giảm spawnRate
      Các hàm con (update, updateCoordinate, updateVelocity, updateDisplayStatus) xử lý vị trí, vận tốc, và trạng thái hiển thị
      */
    void updateEnemy(uint8_t dt) {
        ...
        }
  ```

### KẾT QUẢ

[Video demo Space Invaders](https://drive.google.com/file/d/1ug3VB_5Ezff92ucPog5qQGBh5jKgkAz7/view?usp=sharing)
*Video: Demo trò chơi Space Invader trên STM32F429*
