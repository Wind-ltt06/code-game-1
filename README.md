Họ và tên : Lê Trọng Thức
Mã sinh viên : 24021637
Giới thiệu game: Trong một thành phố bị tàn phá bởi chiến tranh, bạn là hi vọng cuối cùng chống lại làn sóng xe tăng địch không ngừng tiến đến.Hãy điều khiển chiếc xe tăng của mình, phá hủy kẻ thù, bảo vệ căn cứ và trở thành huyền thoại chiến trường!"
Điểm số mong muốn : 8
Tự làm những phần nào, phần nào lấy từ các kiểu tutorial/code mẫu
    Những phần đã làm:
    + main.ccp
    + defs.h
    + graphic.h:
    + Wall.h
    + Player.h
    + Bullet.h
    Những phần lấy mẫu :
    + Enermytank 
    + menu.h
    + menu.cpp

Liệt kê các nguồn tham khảo (code, hình ảnh, âm thanh...)
    + chatgpt, youtube
Dùng các lệnh vẽ hình
    + Vẽ hình chữ nhật (viền):
    + Vẽ hình chữ nhật đầy:
    + Vẽ đường thẳng:
    + Vẽ điểm
  
Texture
  + Dùng SDL_Texture để chèn các ảnh vào các hàm đã vẽ
Background
    + Dùng mapTexture để vẽ background nếu có hình nền
    + Nếu không có thì sẽ vẽ ra màu xám
Event bàn phím
    + DÙng keystate để điều khiển tank và SDL_GetKeyboardState để đọc phím
    + sử dụng SDL_PollEvent để sử lý bắn cũng như exit game
Event chuột

Animation (hoạt hình)
    + sử dụng animation tĩnh để di chuyển tank
Xử lý va chạm
    + va chạm giữa đạn và tường
    + va chạm giữa đạn và player
    + va chạm giữa đạn và enermy
Score (có tính điểm)
    + hàm tính điểm nếu giết được enermy thì sẽ được 100đ
Lưu bảng điểm

Sound
    + hàm music bình thường để thêm music
Sound on off
Background music
    + music sẽ chạy từ đầu đến cuối

Font
    + Font chữ ariel
Menu
    + Menu bao gồm starting, setting, exit

Pause/Resume
    + nếu thua thì game sẽ tự động out ra
Status bar / lives (mạng/máu...)
    + đây là game 1 mạng
Các điểm nổi bật khác
    + Phân chia modul rõ ràng nên khi có việc gì rất dễ fix