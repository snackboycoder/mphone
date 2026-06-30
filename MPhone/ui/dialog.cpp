#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFrame>
#include <QGuiApplication>
#include <QGroupBox>
#include <QIcon>
#include <QKeyEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPlainTextEdit>
#include <QProcess>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QScreen>
#include <QSettings>
#include <QSplitter>
#include <QStandardPaths>
#include <QTabWidget>
#include <QMenu>
#include <QStyle>
#include <QTime>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "config.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "videoform.h"
#include "../groupcontroller/groupcontroller.h"

#ifdef Q_OS_WIN32
#include "../util/winutils.h"
#endif

static QFont mphoneIconFont(int pointSize)
{
    static QString family;
    if (family.isEmpty()) {
        const int fontId = QFontDatabase::addApplicationFont(":/font/fontawesome-webfont.ttf");
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        family = families.isEmpty() ? QString() : families.first();
    }
    QFont font(family);
    font.setPointSize(pointSize);
    return font;
}

static QIcon mphoneIcon(QChar glyph, const QColor &color = QColor("#d7e4f2"))
{
    QPixmap pixmap(40, 40);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setPen(color);
    painter.setFont(mphoneIconFont(18));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QString(glyph));
    painter.end();
    return QIcon(pixmap);
}

static void setMPhoneIcon(QPushButton *button, QChar glyph, const QColor &color = QColor("#d7e4f2"))
{
    if (!button) {
        return;
    }
    button->setIcon(mphoneIcon(glyph, color));
    button->setIconSize(QSize(15, 15));
}

static void bindI18n(QWidget *widget, const QString &key)
{
    if (widget) {
        widget->setProperty("i18nKey", key);
    }
}

static QString mphoneTranslate(const QString &language, const QString &key)
{
    static const QMap<QString, QMap<QString, QString>> translations = {
        {"en", {}},
        {"zh", {
            {"Android fleet operations", "安卓集群控制"},
            {"Refresh", "刷新"}, {"Start all", "全部启动"}, {"Recover failed", "恢复失败"}, {"Stop all", "全部停止"},
            {"Devices", "设备"}, {"Streaming", "镜像中"}, {"Failed", "失败"}, {"Selected", "已选择"}, {"Pending", "等待中"},
            {"Auto refresh", "自动刷新"}, {"Diagnostics", "诊断"}, {"Device canvas", "设备画布"}, {"Operations", "操作"},
            {"All", "全选"}, {"None", "取消"}, {"Invert", "反选"}, {"Sync off", "同步关"}, {"Sync on", "同步开"}, {"Hide devices", "隐藏设备"}, {"Hide inspector", "隐藏面板"},
            {"Selection", "选择"}, {"Stream", "镜像"}, {"Network", "网络"}, {"Scripts", "脚本"},
            {"Name", "名称"}, {"Serial", "序列号"}, {"Bitrate", "码率"}, {"Size", "尺寸"}, {"Format", "格式"},
            {"Rotate", "旋转"}, {"Save path", "保存路径"}, {"Wi-Fi", "无线"}, {"Start USB", "启动USB"},
            {"Start", "启动"}, {"Stop", "停止"}, {"Get IP", "获取IP"}, {"Connect", "连接"}, {"Drop", "断开"},
            {"Record", "录制"}, {"Background", "后台"}, {"Reverse", "反向"}, {"Top", "置顶"},
            {"Screen off", "息屏"}, {"Frameless", "无边框"}, {"Awake", "常亮"}, {"Toolbar", "工具栏"},
            {"Inspector", "检查器"}, {"ADB ready", "ADB就绪"}, {"Queue idle", "队列空闲"}, {"Device Pool", "设备池"}, {"Stream settings", "镜像设置"}, {"USB controls", "USB控制"}, {"Wireless", "无线"},
            {"Control", "控制"}, {"Display", "显示"}, {"Batch", "批量"}, {"Apps / ADB", "应用 / ADB"}, {"Back", "返回"}, {"Home", "主页"}, {"Recent apps", "最近任务"}, {"Power", "电源"}, {"Wake screen", "唤醒屏幕"}, {"Sleep screen", "关闭屏幕"}, {"Volume down", "音量减"}, {"Volume up", "音量加"},
            {"Focus in workspace", "在工作区放大"}, {"Screenshot", "截图"}, {"Reconnect stream", "重连镜像"}, {"Stop stream", "停止镜像"}, {"Select device", "选择设备"}, {"Make sync master", "设为同步主机"}, {"Run current script here", "在此设备运行脚本"}, {"Run current script on selected", "在已选设备运行脚本"},
            {"Current activity", "当前 Activity"}, {"List packages", "应用列表"}, {"Battery status", "电池状态"}, {"Device properties", "设备属性"}, {"Open app switcher", "打开任务切换"}, {"Copy serial to log", "复制序列号到日志"}, {"Health check", "健康检查"},
            {"Queued", "排队中"}, {"Retrying", "重试中"}, {"Recovering", "恢复中"}, {"Starting", "启动中"}, {"Auto-starting stream", "自动启动镜像"}, {"Manual recovery queued", "已加入手动恢复队列"}, {"Video/control handshake did not complete", "视频/控制握手未完成"}, {"Recovering failed stream", "正在恢复失败镜像"},
        }},
        {"vi", {
            {"Android fleet operations", "Điều phối cụm Android"},
            {"Refresh", "Làm mới"}, {"Start all", "Mở tất cả"}, {"Recover failed", "Khôi phục lỗi"}, {"Stop all", "Dừng tất cả"},
            {"Devices", "Thiết bị"}, {"Streaming", "Đang stream"}, {"Failed", "Lỗi"}, {"Selected", "Đã chọn"}, {"Pending", "Đang chờ"},
            {"Auto refresh", "Tự làm mới"}, {"Diagnostics", "Chẩn đoán"}, {"Device canvas", "Màn hình thiết bị"}, {"Operations", "Điều khiển"},
            {"All", "Tất cả"}, {"None", "Bỏ chọn"}, {"Invert", "Đảo chọn"}, {"Sync off", "Tắt đồng bộ"}, {"Sync on", "Bật đồng bộ"}, {"Hide devices", "Ẩn thiết bị"}, {"Hide inspector", "Ẩn điều khiển"},
            {"Selection", "Chọn"}, {"Stream", "Stream"}, {"Network", "Mạng"}, {"Scripts", "Script"},
            {"Name", "Tên"}, {"Serial", "Serial"}, {"Bitrate", "Bitrate"}, {"Size", "Kích thước"}, {"Format", "Định dạng"},
            {"Rotate", "Xoay"}, {"Save path", "Nơi lưu"}, {"Wi-Fi", "Wi-Fi"}, {"Start USB", "Mở USB"},
            {"Start", "Mở"}, {"Stop", "Dừng"}, {"Get IP", "Lấy IP"}, {"Connect", "Kết nối"}, {"Drop", "Ngắt"},
            {"Record", "Ghi"}, {"Background", "Nền"}, {"Reverse", "Đảo chiều"}, {"Top", "Trên cùng"},
            {"Screen off", "Tắt màn"}, {"Frameless", "Không viền"}, {"Awake", "Giữ sáng"}, {"Toolbar", "Thanh công cụ"},
            {"Inspector", "Bảng điều khiển"}, {"ADB ready", "ADB sẵn sàng"}, {"Queue idle", "Hàng đợi rảnh"}, {"Device Pool", "Kho thiết bị"}, {"Stream settings", "Cấu hình stream"}, {"USB controls", "Điều khiển USB"}, {"Wireless", "Không dây"},
            {"Control", "Điều khiển"}, {"Display", "Hiển thị"}, {"Batch", "Hàng loạt"}, {"Apps / ADB", "Ứng dụng / ADB"}, {"Back", "Quay lại"}, {"Home", "Trang chính"}, {"Recent apps", "Đa nhiệm"}, {"Power", "Nguồn"}, {"Wake screen", "Bật màn"}, {"Sleep screen", "Tắt màn"}, {"Volume down", "Giảm âm"}, {"Volume up", "Tăng âm"},
            {"Focus in workspace", "Phóng to trong app"}, {"Screenshot", "Chụp màn"}, {"Reconnect stream", "Kết nối lại stream"}, {"Stop stream", "Dừng stream"}, {"Select device", "Chọn thiết bị"}, {"Make sync master", "Đặt làm máy chủ đồng bộ"}, {"Run current script here", "Chạy script trên máy này"}, {"Run current script on selected", "Chạy script trên máy đã chọn"},
            {"Current activity", "Activity hiện tại"}, {"List packages", "Danh sách package"}, {"Battery status", "Trạng thái pin"}, {"Device properties", "Thuộc tính thiết bị"}, {"Open app switcher", "Mở đa nhiệm"}, {"Copy serial to log", "Ghi serial vào log"}, {"Health check", "Kiểm tra sức khỏe"},
            {"Queued", "Đang xếp hàng"}, {"Retrying", "Đang thử lại"}, {"Recovering", "Đang khôi phục"}, {"Starting", "Đang mở"}, {"Auto-starting stream", "Tự mở stream"}, {"Manual recovery queued", "Đã xếp hàng khôi phục"}, {"Video/control handshake did not complete", "Bắt tay video/điều khiển chưa hoàn tất"}, {"Recovering failed stream", "Đang khôi phục stream lỗi"},
        }},
        {"hi", {
            {"Android fleet operations", "Android फ्लीट संचालन"}, {"Refresh", "रीफ्रेश"}, {"Start all", "सब शुरू"}, {"Recover failed", "फेल रिकवर"}, {"Stop all", "सब रोकें"},
            {"Devices", "डिवाइस"}, {"Streaming", "स्ट्रीमिंग"}, {"Failed", "फेल"}, {"Selected", "चुने गए"}, {"Pending", "प्रतीक्षा"}, {"Auto refresh", "ऑटो रीफ्रेश"},
            {"Diagnostics", "डायग्नोस्टिक्स"}, {"Device canvas", "डिवाइस कैनवास"}, {"Operations", "ऑपरेशन"}, {"All", "सभी"}, {"None", "कोई नहीं"},
            {"Invert", "उलटें"}, {"Sync off", "सिंक बंद"}, {"Sync on", "सिंक चालू"}, {"Hide devices", "डिवाइस छुपाएं"}, {"Hide inspector", "इंस्पेक्टर छुपाएं"}, {"Selection", "चयन"}, {"Stream", "स्ट्रीम"}, {"Network", "नेटवर्क"}, {"Scripts", "स्क्रिप्ट"},
            {"Name", "नाम"}, {"Serial", "सीरियल"}, {"Bitrate", "बिटरेट"}, {"Size", "साइज"}, {"Format", "फॉर्मेट"}, {"Rotate", "घुमाएं"}, {"Save path", "सेव पाथ"},
            {"Start", "शुरू"}, {"Stop", "रोकें"}, {"Connect", "कनेक्ट"}, {"Drop", "ड्रॉप"}, {"Inspector", "इंस्पेक्टर"},
            {"Control", "कंट्रोल"}, {"Display", "डिस्प्ले"}, {"Batch", "बैच"}, {"Apps / ADB", "ऐप्स / ADB"}, {"Back", "बैक"}, {"Home", "होम"}, {"Recent apps", "रीसेंट"}, {"Power", "पावर"}, {"Wake screen", "स्क्रीन जगाएं"}, {"Sleep screen", "स्क्रीन बंद"}, {"Screenshot", "स्क्रीनशॉट"}, {"Reconnect stream", "स्ट्रीम फिर जोड़ें"}, {"Stop stream", "स्ट्रीम रोकें"}, {"Select device", "डिवाइस चुनें"}, {"Health check", "हेल्थ चेक"}, {"Queued", "कतार में"}, {"Retrying", "फिर कोशिश"}, {"Recovering", "रिकवर हो रहा"}, {"Starting", "शुरू हो रहा"},
        }},
        {"ms", {
            {"Android fleet operations", "Operasi kumpulan Android"}, {"Refresh", "Segar semula"}, {"Start all", "Mula semua"}, {"Recover failed", "Pulih gagal"}, {"Stop all", "Henti semua"},
            {"Devices", "Peranti"}, {"Streaming", "Menstrim"}, {"Failed", "Gagal"}, {"Selected", "Dipilih"}, {"Pending", "Menunggu"}, {"Auto refresh", "Auto segar"},
            {"Diagnostics", "Diagnostik"}, {"Device canvas", "Kanvas peranti"}, {"Operations", "Operasi"}, {"All", "Semua"}, {"None", "Tiada"},
            {"Invert", "Songsang"}, {"Sync off", "Sync mati"}, {"Sync on", "Sync hidup"}, {"Hide devices", "Sembunyi peranti"}, {"Hide inspector", "Sembunyi panel"}, {"Selection", "Pilihan"}, {"Stream", "Strim"}, {"Network", "Rangkaian"}, {"Scripts", "Skrip"},
            {"Name", "Nama"}, {"Serial", "Siri"}, {"Bitrate", "Kadar bit"}, {"Size", "Saiz"}, {"Format", "Format"}, {"Rotate", "Putar"}, {"Save path", "Laluan simpan"},
            {"Start", "Mula"}, {"Stop", "Henti"}, {"Connect", "Sambung"}, {"Drop", "Putus"}, {"Inspector", "Panel"},
            {"Control", "Kawal"}, {"Display", "Paparan"}, {"Batch", "Pukal"}, {"Apps / ADB", "Aplikasi / ADB"}, {"Back", "Kembali"}, {"Home", "Utama"}, {"Recent apps", "Aplikasi terkini"}, {"Power", "Kuasa"}, {"Wake screen", "Hidupkan skrin"}, {"Sleep screen", "Matikan skrin"}, {"Screenshot", "Tangkapan skrin"}, {"Reconnect stream", "Sambung semula strim"}, {"Stop stream", "Henti strim"}, {"Select device", "Pilih peranti"}, {"Health check", "Semak kesihatan"}, {"Queued", "Dalam giliran"}, {"Retrying", "Cuba semula"}, {"Recovering", "Memulih"}, {"Starting", "Memula"},
        }},
        {"sg", {
            {"Android fleet operations", "Android fleet operations"}, {"Devices", "Devices"}, {"Inspector", "Inspector"},
        }},
        {"lo", {
            {"Android fleet operations", "ຄວບຄຸມກຸ່ມ Android"}, {"Refresh", "ໂຫຼດໃໝ່"}, {"Start all", "ເລີ່ມທັງໝົດ"}, {"Recover failed", "ກູ້ຄືນທີ່ລົ້ມເຫຼວ"}, {"Stop all", "ຢຸດທັງໝົດ"},
            {"Devices", "ອຸປະກອນ"}, {"Streaming", "ກຳລັງສະແດງ"}, {"Failed", "ລົ້ມເຫຼວ"}, {"Selected", "ເລືອກແລ້ວ"}, {"Auto refresh", "ອັບເດດອັດຕະໂນມັດ"},
            {"Diagnostics", "ກວດສອບ"}, {"Device canvas", "ໜ້າຈໍອຸປະກອນ"}, {"Operations", "ຄວບຄຸມ"}, {"All", "ທັງໝົດ"}, {"None", "ບໍ່ເລືອກ"},
            {"Selection", "ເລືອກ"}, {"Stream", "ສະແດງ"}, {"Network", "ເຄືອຂ່າຍ"}, {"Scripts", "ສະຄຣິບ"}, {"Start", "ເລີ່ມ"}, {"Stop", "ຢຸດ"}, {"Connect", "ເຊື່ອມຕໍ່"},
        }},
        {"th", {
            {"Android fleet operations", "ควบคุมฟาร์ม Android"}, {"Refresh", "รีเฟรช"}, {"Start all", "เริ่มทั้งหมด"}, {"Recover failed", "กู้คืนที่ล้มเหลว"}, {"Stop all", "หยุดทั้งหมด"},
            {"Devices", "อุปกรณ์"}, {"Streaming", "กำลังสตรีม"}, {"Failed", "ล้มเหลว"}, {"Selected", "เลือกแล้ว"}, {"Pending", "รอดำเนินการ"}, {"Auto refresh", "รีเฟรชอัตโนมัติ"},
            {"Diagnostics", "วินิจฉัย"}, {"Device canvas", "หน้าจออุปกรณ์"}, {"Operations", "การทำงาน"}, {"All", "ทั้งหมด"}, {"None", "ไม่เลือก"},
            {"Invert", "กลับเลือก"}, {"Sync off", "ปิดซิงค์"}, {"Sync on", "เปิดซิงค์"}, {"Hide devices", "ซ่อนอุปกรณ์"}, {"Hide inspector", "ซ่อนแผง"}, {"Selection", "เลือก"}, {"Stream", "สตรีม"}, {"Network", "เครือข่าย"}, {"Scripts", "สคริปต์"},
            {"Name", "ชื่อ"}, {"Serial", "ซีเรียล"}, {"Start", "เริ่ม"}, {"Stop", "หยุด"}, {"Connect", "เชื่อมต่อ"}, {"Drop", "ตัดการเชื่อมต่อ"},
            {"Control", "ควบคุม"}, {"Display", "แสดงผล"}, {"Batch", "ชุดคำสั่ง"}, {"Apps / ADB", "แอป / ADB"}, {"Back", "ย้อนกลับ"}, {"Home", "หน้าหลัก"}, {"Recent apps", "แอปล่าสุด"}, {"Power", "พาวเวอร์"}, {"Screenshot", "จับภาพ"}, {"Reconnect stream", "เชื่อมต่อสตรีมใหม่"}, {"Stop stream", "หยุดสตรีม"}, {"Select device", "เลือกอุปกรณ์"}, {"Health check", "ตรวจสุขภาพ"}, {"Queued", "เข้าคิว"}, {"Retrying", "ลองใหม่"}, {"Recovering", "กำลังกู้คืน"}, {"Starting", "กำลังเริ่ม"},
        }},
        {"id", {
            {"Android fleet operations", "Operasi farm Android"}, {"Refresh", "Segarkan"}, {"Start all", "Mulai semua"}, {"Recover failed", "Pulihkan gagal"}, {"Stop all", "Hentikan semua"},
            {"Devices", "Perangkat"}, {"Streaming", "Streaming"}, {"Failed", "Gagal"}, {"Selected", "Dipilih"}, {"Pending", "Menunggu"}, {"Auto refresh", "Segar otomatis"},
            {"Diagnostics", "Diagnostik"}, {"Device canvas", "Kanvas perangkat"}, {"Operations", "Operasi"}, {"All", "Semua"}, {"None", "Kosong"},
            {"Invert", "Balik"}, {"Sync off", "Sync mati"}, {"Sync on", "Sync hidup"}, {"Hide devices", "Sembunyikan perangkat"}, {"Hide inspector", "Sembunyikan panel"}, {"Selection", "Pilihan"}, {"Stream", "Stream"}, {"Network", "Jaringan"}, {"Scripts", "Skrip"},
            {"Name", "Nama"}, {"Serial", "Serial"}, {"Start", "Mulai"}, {"Stop", "Stop"}, {"Connect", "Hubungkan"}, {"Drop", "Putus"},
            {"Control", "Kontrol"}, {"Display", "Tampilan"}, {"Batch", "Batch"}, {"Apps / ADB", "Aplikasi / ADB"}, {"Back", "Kembali"}, {"Home", "Beranda"}, {"Recent apps", "Aplikasi terbaru"}, {"Power", "Daya"}, {"Screenshot", "Screenshot"}, {"Reconnect stream", "Sambung ulang stream"}, {"Stop stream", "Hentikan stream"}, {"Select device", "Pilih perangkat"}, {"Health check", "Cek kesehatan"}, {"Queued", "Antre"}, {"Retrying", "Mencoba lagi"}, {"Recovering", "Memulihkan"}, {"Starting", "Memulai"},
        }},
        {"fil", {
            {"Android fleet operations", "Android fleet operations"}, {"Refresh", "I-refresh"}, {"Start all", "Simulan lahat"}, {"Recover failed", "Ayusin failed"}, {"Stop all", "Itigil lahat"},
            {"Devices", "Devices"}, {"Streaming", "Streaming"}, {"Failed", "Failed"}, {"Selected", "Napili"}, {"Auto refresh", "Auto refresh"},
            {"Diagnostics", "Diagnostics"}, {"Device canvas", "Device canvas"}, {"Operations", "Operations"}, {"All", "Lahat"}, {"None", "Wala"},
            {"Selection", "Selection"}, {"Stream", "Stream"}, {"Network", "Network"}, {"Scripts", "Scripts"}, {"Start", "Start"}, {"Stop", "Stop"}, {"Connect", "Connect"},
        }},
        {"km", {
            {"Android fleet operations", "គ្រប់គ្រងក្រុម Android"}, {"Refresh", "ធ្វើថ្មី"}, {"Start all", "ចាប់ផ្តើមទាំងអស់"}, {"Recover failed", "ស្តារបរាជ័យ"}, {"Stop all", "បញ្ឈប់ទាំងអស់"},
            {"Devices", "ឧបករណ៍"}, {"Streaming", "កំពុងបង្ហាញ"}, {"Failed", "បរាជ័យ"}, {"Selected", "បានជ្រើស"}, {"Operations", "ប្រតិបត្តិការ"}, {"Scripts", "ស្គ្រីប"},
        }},
        {"my", {
            {"Android fleet operations", "Android ဖုန်းအုပ်စု စီမံခန့်ခွဲမှု"}, {"Refresh", "ပြန်လည်စစ်ဆေး"}, {"Start all", "အားလုံးစတင်"}, {"Recover failed", "မအောင်မြင်သည် ပြန်စမ်း"}, {"Stop all", "အားလုံးရပ်"},
            {"Devices", "စက်များ"}, {"Streaming", "ပြသနေ"}, {"Failed", "မအောင်မြင်"}, {"Selected", "ရွေးချယ်ပြီး"}, {"Operations", "လုပ်ဆောင်ချက်"}, {"Scripts", "စက్రစ်"},
        }},
        {"ja", {
            {"Android fleet operations", "Android端末群オペレーション"}, {"Refresh", "更新"}, {"Start all", "すべて開始"}, {"Recover failed", "失敗を復旧"}, {"Stop all", "すべて停止"},
            {"Devices", "端末"}, {"Streaming", "配信中"}, {"Failed", "失敗"}, {"Selected", "選択済み"}, {"Pending", "待機中"}, {"Operations", "操作"}, {"Scripts", "スクリプト"}, {"Auto refresh", "自動更新"},
            {"Diagnostics", "診断"}, {"Device canvas", "端末キャンバス"}, {"All", "すべて"}, {"None", "解除"}, {"Invert", "反転"}, {"Sync off", "同期オフ"}, {"Sync on", "同期オン"}, {"Selection", "選択"}, {"Stream", "配信"}, {"Network", "ネットワーク"},
            {"Control", "制御"}, {"Display", "表示"}, {"Batch", "一括"}, {"Apps / ADB", "アプリ / ADB"}, {"Back", "戻る"}, {"Home", "ホーム"}, {"Recent apps", "履歴"}, {"Power", "電源"}, {"Screenshot", "スクリーンショット"}, {"Reconnect stream", "配信を再接続"}, {"Stop stream", "配信停止"}, {"Select device", "端末を選択"}, {"Health check", "ヘルスチェック"}, {"Queued", "キュー中"}, {"Retrying", "再試行中"}, {"Recovering", "復旧中"}, {"Starting", "開始中"},
        }},
        {"ko", {
            {"Android fleet operations", "Android 디바이스 팜 운영"}, {"Refresh", "새로고침"}, {"Start all", "전체 시작"}, {"Recover failed", "실패 복구"}, {"Stop all", "전체 중지"},
            {"Devices", "기기"}, {"Streaming", "스트리밍"}, {"Failed", "실패"}, {"Selected", "선택됨"}, {"Pending", "대기"}, {"Operations", "작업"}, {"Scripts", "스크립트"}, {"Auto refresh", "자동 새로고침"},
            {"Diagnostics", "진단"}, {"Device canvas", "기기 화면"}, {"All", "전체"}, {"None", "해제"}, {"Invert", "반전"}, {"Sync off", "동기화 꺼짐"}, {"Sync on", "동기화 켜짐"}, {"Selection", "선택"}, {"Stream", "스트림"}, {"Network", "네트워크"},
            {"Control", "제어"}, {"Display", "표시"}, {"Batch", "일괄"}, {"Apps / ADB", "앱 / ADB"}, {"Back", "뒤로"}, {"Home", "홈"}, {"Recent apps", "최근 앱"}, {"Power", "전원"}, {"Screenshot", "스크린샷"}, {"Reconnect stream", "스트림 재연결"}, {"Stop stream", "스트림 중지"}, {"Select device", "기기 선택"}, {"Health check", "상태 확인"}, {"Queued", "대기열"}, {"Retrying", "재시도"}, {"Recovering", "복구 중"}, {"Starting", "시작 중"},
        }},
    };

    const auto languageMap = translations.value(language);
    return languageMap.value(key, translations.value("en").value(key, key));
}

static QString mphonePreferencePath()
{
    QString root = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (root.isEmpty()) {
        root = QDir::homePath() + "/.config/MPhone";
    }
    QDir dir(root);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath("preferences.ini");
}

static void clearLayoutItems(QLayout *layout)
{
    if (!layout) {
        return;
    }

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->setParent(nullptr);
        }
        if (QLayout *childLayout = item->layout()) {
            clearLayoutItems(childLayout);
        }
        delete item;
    }
}

static void replaceWidgetLayout(QWidget *widget, QLayout *layout)
{
    if (!widget || !layout) {
        delete layout;
        return;
    }

    if (QLayout *oldLayout = widget->layout()) {
        clearLayoutItems(oldLayout);
        delete oldLayout;
    }
    widget->setLayout(layout);
}

static QLabel *makePanelLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName("fieldLabel");
    bindI18n(label, text);
    label->setMinimumWidth(78);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    return label;
}

Dialog::Dialog(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    initUI();
    initMPhoneView();

    updateBootConfig(true);

    on_useSingleModeCheck_clicked();

    connect(&m_autoUpdatetimer, &QTimer::timeout, this, &Dialog::on_updateDevice_clicked);
    if (ui->autoUpdatecheckBox->isChecked()) {
        m_autoUpdatetimer.start(5000);
    }

    connect(&m_adb, &qsc::AdbProcess::adbProcessResult, this, [this](qsc::AdbProcess::ADB_EXEC_RESULT processResult) {
        QString log = "";
        bool newLine = true;
        QStringList args = m_adb.arguments();

        switch (processResult) {
        case qsc::AdbProcess::AER_ERROR_START:
            break;
        case qsc::AdbProcess::AER_SUCCESS_START:
            log = "adb run";
            newLine = false;
            break;
        case qsc::AdbProcess::AER_ERROR_EXEC:
            //log = m_adb.getErrorOut();
            if (args.contains("ifconfig") && args.contains("wlan0")) {
                getIPbyIp();
            }
            break;
        case qsc::AdbProcess::AER_ERROR_MISSING_BINARY:
            log = "adb not found";
            break;
        case qsc::AdbProcess::AER_SUCCESS_EXEC:
            //log = m_adb.getStdOut();
            if (args.contains("devices")) {
                QStringList devices = m_adb.getDevicesSerialFromStdOut();
                ui->serialBox->clear();
                ui->connectedPhoneList->clear();
                int deviceIndex = 0;
                for (auto &item : devices) {
                    ui->serialBox->addItem(item);
                    ui->connectedPhoneList->addItem(Config::getInstance().getNickName(item) + "-" + item);
                    if (!m_agentCheckedDevices.contains(item)) {
                        const int delayMs = deviceIndex * 5;
                        QTimer::singleShot(delayMs, this, [this, serial = item]() {
                            ensureMPhoneAgent(serial);
                        });
                    }
                    if (!qsc::IDeviceManage::getInstance().getDevice(item) &&
                        !m_mphoneCards.contains(item) &&
                        !m_startQueue.contains(item)) {
                        m_startQueue.append(item);
                        setMPhoneCard(item, makeStatusCard(item, "Queued", "Auto-starting stream"));
                    }
                    ++deviceIndex;
                }
                updateFleetMetrics();
                applyDevicePoolFilter();
                pumpDeviceStartQueue();
            } else if (args.contains("show") && args.contains("wlan0")) {
                QString ip = m_adb.getDeviceIPFromStdOut();
                if (ip.isEmpty()) {
                    log = "ip not find, connect to wifi?";
                    break;
                }
                ui->deviceIpEdt->setEditText(ip);
            } else if (args.contains("ifconfig") && args.contains("wlan0")) {
                QString ip = m_adb.getDeviceIPFromStdOut();
                if (ip.isEmpty()) {
                    log = "ip not find, connect to wifi?";
                    break;
                }
                ui->deviceIpEdt->setEditText(ip);
            } else if (args.contains("ip -o a")) {
                QString ip = m_adb.getDeviceIPByIpFromStdOut();
                if (ip.isEmpty()) {
                    log = "ip not find, connect to wifi?";
                    break;
                }
                ui->deviceIpEdt->setEditText(ip);
            }
            break;
        }
        if (!log.isEmpty()) {
            outLog(log, newLine);
        }
        updateFleetMetrics();
    });

    m_hideIcon = new QSystemTrayIcon(this);
    m_hideIcon->setIcon(QIcon(":/image/tray/logo.png"));
    m_menu = new QMenu(this);
    m_quit = new QAction(this);
    m_showWindow = new QAction(this);
    m_showWindow->setText(tr("show"));
    m_quit->setText(tr("quit"));
    m_menu->addAction(m_showWindow);
    m_menu->addAction(m_quit);
    m_hideIcon->setContextMenu(m_menu);
    m_hideIcon->show();
    connect(m_showWindow, &QAction::triggered, this, &Dialog::show);
    connect(m_quit, &QAction::triggered, this, [this]() {
        m_hideIcon->hide();
        qApp->quit();
    });
    connect(m_hideIcon, &QSystemTrayIcon::activated, this, &Dialog::slotActivated);

    connect(&qsc::IDeviceManage::getInstance(), &qsc::IDeviceManage::deviceConnected, this, &Dialog::onDeviceConnected);
    connect(&qsc::IDeviceManage::getInstance(), &qsc::IDeviceManage::deviceDisconnected, this, &Dialog::onDeviceDisconnected);

    on_updateDevice_clicked();
}

Dialog::~Dialog()
{
    qDebug() << "~Dialog()";
    updateBootConfig(false);
    qsc::IDeviceManage::getInstance().disconnectAllDevice();
    delete ui;
}

void Dialog::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    //setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    setWindowTitle(Config::getInstance().getTitle());
#ifdef Q_OS_LINUX
    // Set window icon (inherits from application icon set in main.cpp)
    // If application icon was set, this will use it automatically
    if (!qApp->windowIcon().isNull()) {
        setWindowIcon(qApp->windowIcon());
    }
#endif

#ifdef Q_OS_WIN32
    WinUtils::setDarkBorderToWindow((HWND)this->winId(), true);
#endif

    ui->bitRateEdit->setValidator(new QIntValidator(1, 99999, this));

    ui->maxSizeBox->addItem("640");
    ui->maxSizeBox->addItem("720");
    ui->maxSizeBox->addItem("1080");
    ui->maxSizeBox->addItem("1280");
    ui->maxSizeBox->addItem("1920");
    ui->maxSizeBox->addItem(tr("original"));

    ui->formatBox->addItem("mp4");
    ui->formatBox->addItem("mkv");

    ui->lockOrientationBox->addItem(tr("no lock"));
    ui->lockOrientationBox->addItem("0");
    ui->lockOrientationBox->addItem("90");
    ui->lockOrientationBox->addItem("180");
    ui->lockOrientationBox->addItem("270");
    ui->lockOrientationBox->setCurrentIndex(0);

    // 加载IP历史记录
    loadIpHistory();

    // 加载端口历史记录
    loadPortHistory();

    // 为deviceIpEdt添加右键菜单
    if (ui->deviceIpEdt->lineEdit()) {
        ui->deviceIpEdt->lineEdit()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->deviceIpEdt->lineEdit(), &QWidget::customContextMenuRequested,
                this, &Dialog::showIpEditMenu);
    }
    
    // 为devicePortEdt添加右键菜单
    if (ui->devicePortEdt->lineEdit()) {
        ui->devicePortEdt->lineEdit()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->devicePortEdt->lineEdit(), &QWidget::customContextMenuRequested,
                this, &Dialog::showPortEditMenu);
    }
}

void Dialog::initMPhoneView()
{
    setWindowTitle("MPhone");
    QSettings preferences(mphonePreferencePath(), QSettings::IniFormat);
    const QString savedLanguage = preferences.value("language/code", "en").toString();
    if (!savedLanguage.isEmpty()) {
        m_languageCode = savedLanguage;
    }
    localizeMPhoneUi();
    rebuildMPhoneConfigPanel();
    initBatchScriptPanel();
    applyMPhoneStyle();

    QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    setGeometry(availableGeometry.adjusted(16, 16, -16, -16));

    auto *shellLayout = new QVBoxLayout();
    shellLayout->setContentsMargins(0, 0, 0, 0);
    shellLayout->setSpacing(0);
    replaceWidgetLayout(this, shellLayout);

    auto *topBar = new QFrame(this);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(14, 0, 14, 0);
    topLayout->setSpacing(10);
    auto *brand = new QLabel("MPhone", topBar);
    brand->setObjectName("AppBrand");
    auto *subtitle = new QLabel("Android fleet operations", topBar);
    bindI18n(subtitle, "Android fleet operations");
    subtitle->setObjectName("AppSubtitle");
    m_totalDevicesMetric = new QLabel("Devices 0", topBar);
    m_runningDevicesMetric = new QLabel("Streaming 0", topBar);
    m_failedDevicesMetric = new QLabel("Failed 0", topBar);
    m_selectedDevicesMetric = new QLabel("Selected 0", topBar);
    m_totalDevicesMetric->setObjectName("MetricChip");
    m_runningDevicesMetric->setObjectName("MetricChip");
    m_failedDevicesMetric->setObjectName("MetricChip");
    m_selectedDevicesMetric->setObjectName("MetricChip");
    topLayout->addWidget(brand, 0);
    topLayout->addWidget(subtitle, 0);
    topLayout->addSpacing(12);
    topLayout->addWidget(m_totalDevicesMetric, 0);
    topLayout->addWidget(m_runningDevicesMetric, 0);
    topLayout->addWidget(m_failedDevicesMetric, 0);
    topLayout->addWidget(m_selectedDevicesMetric, 0);
    topLayout->addStretch(1);

    auto *refreshTopButton = new QPushButton("Refresh", topBar);
    auto *startTopButton = new QPushButton("Start all", topBar);
    auto *recoverTopButton = new QPushButton("Recover failed", topBar);
    auto *stopTopButton = new QPushButton("Stop all", topBar);
    bindI18n(refreshTopButton, "Refresh");
    bindI18n(startTopButton, "Start all");
    bindI18n(recoverTopButton, "Recover failed");
    bindI18n(stopTopButton, "Stop all");
    refreshTopButton->setToolTip("Refresh connected devices");
    startTopButton->setToolTip("Start all online device screens");
    recoverTopButton->setToolTip("Retry all failed device streams");
    stopTopButton->setToolTip("Stop all running device screens");
    setMPhoneIcon(refreshTopButton, QChar(0xf021));
    setMPhoneIcon(startTopButton, QChar(0xf04b));
    setMPhoneIcon(recoverTopButton, QChar(0xf0e7), QColor("#ffd166"));
    setMPhoneIcon(stopTopButton, QChar(0xf04d), QColor("#ff9a9a"));
    startTopButton->setObjectName("primaryButton");
    stopTopButton->setObjectName("dangerButton");
    connect(refreshTopButton, &QPushButton::clicked, this, &Dialog::on_updateDevice_clicked);
    connect(startTopButton, &QPushButton::clicked, this, &Dialog::startAllDeviceScreens);
    connect(recoverTopButton, &QPushButton::clicked, this, [this]() {
        int recovered = 0;
        for (const QString &serial : m_mphoneCards.keys()) {
            QWidget *card = m_mphoneCards.value(serial);
            if (!card || card->property("state").toString() != "Failed") {
                continue;
            }
            QWidget *oldCard = m_mphoneCards.take(serial);
            if (oldCard) {
                oldCard->deleteLater();
            }
            m_startAttempts[serial] = 0;
            m_recoverAttempts[serial] = 0;
            setMPhoneCard(serial, makeStatusCard(serial, "Queued", "Manual recovery queued"));
            if (!m_startQueue.contains(serial)) {
                m_startQueue.append(serial);
            }
            ++recovered;
        }
        outLog(QString("recover failed queued: %1").arg(recovered), false);
        pumpDeviceStartQueue();
        updateFleetMetrics();
    });
    connect(stopTopButton, &QPushButton::clicked, this, &Dialog::on_stopAllServerBtn_clicked);
    m_languageBox = new QComboBox(topBar);
    m_languageBox->setObjectName("LanguageBox");
    m_languageBox->setToolTip("Language");
    m_languageBox->addItem("English", "en");
    m_languageBox->addItem("Chinese", "zh");
    m_languageBox->addItem("Vietnamese", "vi");
    m_languageBox->addItem("Hindi", "hi");
    m_languageBox->addItem("Malay", "ms");
    m_languageBox->addItem("Singapore", "sg");
    m_languageBox->addItem("Lao", "lo");
    m_languageBox->addItem("Thai", "th");
    m_languageBox->addItem("Indonesian", "id");
    m_languageBox->addItem("Filipino", "fil");
    m_languageBox->addItem("Khmer", "km");
    m_languageBox->addItem("Burmese", "my");
    m_languageBox->addItem("Japanese", "ja");
    m_languageBox->addItem("Korean", "ko");
    m_languageBox->setMinimumWidth(150);
    m_languageBox->setMaximumWidth(210);
    const int savedLanguageIndex = m_languageBox->findData(m_languageCode);
    if (savedLanguageIndex >= 0) {
        m_languageBox->setCurrentIndex(savedLanguageIndex);
    }
    connect(m_languageBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (!m_languageBox || index < 0) {
            return;
        }
        m_languageCode = m_languageBox->itemData(index).toString();
        QSettings preferences(mphonePreferencePath(), QSettings::IniFormat);
        preferences.setValue("language/code", m_languageCode);
        preferences.sync();
        applyMPhoneLanguage();
    });
    topLayout->addWidget(refreshTopButton, 0);
    topLayout->addWidget(startTopButton, 0);
    topLayout->addWidget(recoverTopButton, 0);
    topLayout->addWidget(stopTopButton, 0);
    topLayout->addWidget(m_languageBox, 0);
    shellLayout->addWidget(topBar, 0);

    auto *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setObjectName("MainSplitter");
    mainSplitter->setChildrenCollapsible(false);

    auto *navigationShell = new QWidget(mainSplitter);
    navigationShell->setObjectName("NavigationShell");
    navigationShell->setMinimumWidth(374);
    navigationShell->setMaximumWidth(480);
    auto *navigationLayout = new QHBoxLayout(navigationShell);
    navigationLayout->setContentsMargins(0, 0, 0, 0);
    navigationLayout->setSpacing(0);

    auto *navRail = new QFrame(navigationShell);
    navRail->setObjectName("NavRail");
    navRail->setFixedWidth(144);
    auto *navRailLayout = new QVBoxLayout(navRail);
    navRailLayout->setContentsMargins(8, 12, 8, 12);
    navRailLayout->setSpacing(8);
    auto makeRailButton = [navRail](const QString &label, QChar icon) {
        auto *button = new QPushButton(label, navRail);
        button->setObjectName("NavRailButton");
        button->setToolTip(label);
        button->setFixedSize(120, 58);
        setMPhoneIcon(button, icon, QColor("#0b8bdc"));
        return button;
    };
    auto *railDeviceButton = makeRailButton("Device", QChar(0xf10b));
    auto *railInfoButton = makeRailButton("Info", QChar(0xf05a));
    auto *railScriptButton = makeRailButton("Script", QChar(0xf121));
    auto *railFileButton = makeRailButton("Files", QChar(0xf0c5));
    auto *railSyncButton = makeRailButton("Sync", QChar(0xf0ec));
    auto *railSettingsButton = makeRailButton("Setup", QChar(0xf013));
    railDeviceButton->setProperty("active", true);
    const QList<QPushButton *> railButtons = {
        railDeviceButton, railInfoButton, railScriptButton,
        railFileButton, railSyncButton, railSettingsButton,
    };
    auto setRailActive = [railButtons](QPushButton *activeButton) {
        for (QPushButton *button : railButtons) {
            button->setProperty("active", button == activeButton);
            button->style()->unpolish(button);
            button->style()->polish(button);
            button->update();
        }
    };
    connect(railDeviceButton, &QPushButton::clicked, this, [this, railDeviceButton, setRailActive]() {
        setRailActive(railDeviceButton);
        if (m_rightConfigScroll && m_rightConfigScroll->isVisible()) {
            m_rightConfigScroll->setVisible(false);
            m_toggleRightButton->setText(i18n("Inspector"));
        }
        if (m_devicePoolPanel && !m_devicePoolPanel->isVisible()) {
            m_devicePoolPanel->setVisible(true);
        }
        if (m_devicePoolPanel) {
            if (QWidget *shell = m_devicePoolPanel->parentWidget()) {
                shell->setMinimumWidth(374);
                shell->setMaximumWidth(480);
            }
        }
        m_toggleLeftButton->setText(i18n("Hide devices"));
        relayoutMPhoneScreens();
    });
    connect(railInfoButton, &QPushButton::clicked, this, [this, railInfoButton, setRailActive]() {
        setRailActive(railInfoButton);
        if (m_devicePoolPanel && m_devicePoolPanel->isVisible()) {
            m_devicePoolPanel->setVisible(false);
            if (QWidget *shell = m_devicePoolPanel->parentWidget()) {
                shell->setMinimumWidth(144);
                shell->setMaximumWidth(144);
            }
            m_toggleLeftButton->setText(i18n("Devices"));
        }
        if (m_rightConfigScroll && !m_rightConfigScroll->isVisible()) {
            m_rightConfigScroll->setVisible(true);
            m_toggleRightButton->setText(i18n("Hide inspector"));
        }
        if (m_operationsTabs) {
            m_operationsTabs->setCurrentIndex(0);
        }
        relayoutMPhoneScreens();
    });
    connect(railScriptButton, &QPushButton::clicked, this, [this, railScriptButton, setRailActive]() {
        setRailActive(railScriptButton);
        if (m_devicePoolPanel && m_devicePoolPanel->isVisible()) {
            m_devicePoolPanel->setVisible(false);
            if (QWidget *shell = m_devicePoolPanel->parentWidget()) {
                shell->setMinimumWidth(144);
                shell->setMaximumWidth(144);
            }
            m_toggleLeftButton->setText(i18n("Devices"));
        }
        if (m_rightConfigScroll && !m_rightConfigScroll->isVisible()) {
            m_rightConfigScroll->setVisible(true);
            m_toggleRightButton->setText(i18n("Hide inspector"));
        }
        if (m_operationsTabs) {
            m_operationsTabs->setCurrentIndex(3);
        }
        relayoutMPhoneScreens();
    });
    connect(railFileButton, &QPushButton::clicked, this, [this, railFileButton, setRailActive]() {
        setRailActive(railFileButton);
        outLog("Files: choose devices, then use push/pull commands from the device context menu", false);
    });
    connect(railSyncButton, &QPushButton::clicked, this, [this, railSyncButton, setRailActive]() {
        setRailActive(railSyncButton);
        m_syncControlEnabled = !m_syncControlEnabled;
        updateFleetSelectionUi();
    });
    connect(railSettingsButton, &QPushButton::clicked, this, [this, railSettingsButton, setRailActive]() {
        setRailActive(railSettingsButton);
        if (m_rightConfigScroll && !m_rightConfigScroll->isVisible()) {
            m_rightConfigScroll->setVisible(true);
            m_toggleRightButton->setText(i18n("Hide inspector"));
        }
        if (m_operationsTabs) {
            m_operationsTabs->setCurrentIndex(1);
        }
        relayoutMPhoneScreens();
    });
    navRailLayout->addWidget(railDeviceButton);
    navRailLayout->addWidget(railInfoButton);
    navRailLayout->addWidget(railScriptButton);
    navRailLayout->addWidget(railFileButton);
    navRailLayout->addWidget(railSyncButton);
    navRailLayout->addStretch(1);
    navRailLayout->addWidget(railSettingsButton);

    auto *devicePoolPanel = new QFrame(navigationShell);
    m_devicePoolPanel = devicePoolPanel;
    devicePoolPanel->setObjectName("DevicePoolPanel");
    devicePoolPanel->setMinimumWidth(230);
    devicePoolPanel->setMaximumWidth(340);
    auto *poolLayout = new QVBoxLayout(devicePoolPanel);
    poolLayout->setContentsMargins(12, 12, 12, 12);
    poolLayout->setSpacing(10);
    auto *poolHeader = new QHBoxLayout();
    auto *poolTitle = new QLabel("Devices", devicePoolPanel);
    poolTitle->setObjectName("PanelTitle");
    bindI18n(poolTitle, "Devices");
    poolHeader->addWidget(poolTitle);
    poolHeader->addStretch(1);
    poolHeader->addWidget(ui->autoUpdatecheckBox);
    poolLayout->addLayout(poolHeader);
    m_poolSummaryLabel = new QLabel("0 devices · 0 streaming · 0 selected", devicePoolPanel);
    m_poolSummaryLabel->setObjectName("PanelMeta");
    poolLayout->addWidget(m_poolSummaryLabel);

    m_deviceSearchEdit = new QLineEdit(devicePoolPanel);
    m_deviceSearchEdit->setObjectName("DeviceSearch");
    m_deviceSearchEdit->setPlaceholderText("Search serial, name, IP");
    m_deviceSearchEdit->setClearButtonEnabled(true);
    poolLayout->addWidget(m_deviceSearchEdit);

    m_deviceFilterBox = new QComboBox(devicePoolPanel);
    m_deviceFilterBox->setObjectName("DeviceFilter");
    m_deviceFilterBox->addItem("All devices", "all");
    m_deviceFilterBox->addItem("Online only", "online");
    m_deviceFilterBox->addItem("Streaming", "streaming");
    m_deviceFilterBox->addItem("Selected", "selected");
    m_deviceFilterBox->addItem("Pending", "pending");
    m_deviceFilterBox->addItem("Failed", "failed");
    poolLayout->addWidget(m_deviceFilterBox);

    auto *connectRow = new QHBoxLayout();
    connectRow->setSpacing(8);
    connectRow->addWidget(ui->wifiConnectBtn, 1);
    connectRow->addWidget(ui->usbConnectBtn, 1);
    poolLayout->addLayout(connectRow);
    ui->connectedPhoneList->setParent(devicePoolPanel);
    ui->connectedPhoneList->setMinimumHeight(220);
    poolLayout->addWidget(ui->connectedPhoneList, 2);
    connect(m_deviceSearchEdit, &QLineEdit::textChanged, this, &Dialog::applyDevicePoolFilter);
    connect(m_deviceFilterBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        applyDevicePoolFilter();
    });

    auto *diagnosticsTitle = new QLabel("Diagnostics", devicePoolPanel);
    diagnosticsTitle->setObjectName("PanelTitle");
    bindI18n(diagnosticsTitle, "Diagnostics");
    poolLayout->addWidget(diagnosticsTitle);
    auto *adbRow = new QHBoxLayout();
    adbRow->setSpacing(6);
    adbRow->addWidget(ui->adbCommandEdt, 1);
    adbRow->addWidget(ui->adbCommandBtn, 0);
    adbRow->addWidget(ui->stopAdbBtn, 0);
    adbRow->addWidget(ui->clearOut, 0);
    poolLayout->addLayout(adbRow);
    ui->outEdit->setParent(devicePoolPanel);
    poolLayout->addWidget(ui->outEdit, 3);
    ui->leftWidget->hide();
    navigationLayout->addWidget(navRail, 0);
    navigationLayout->addWidget(devicePoolPanel, 1);

    auto *workspace = new QWidget(mainSplitter);
    workspace->setObjectName("FleetWorkspace");
    auto *workspaceLayout = new QVBoxLayout(workspace);
    workspaceLayout->setContentsMargins(10, 10, 10, 10);
    workspaceLayout->setSpacing(8);

    auto *workspaceHeader = new QFrame(workspace);
    workspaceHeader->setObjectName("WorkspaceHeader");
    auto *workspaceHeaderLayout = new QHBoxLayout(workspaceHeader);
    workspaceHeaderLayout->setContentsMargins(10, 8, 10, 8);
    workspaceHeaderLayout->setSpacing(8);
    auto *workspaceTitle = new QLabel("Device canvas", workspaceHeader);
    workspaceTitle->setObjectName("WorkspaceTitle");
    bindI18n(workspaceTitle, "Device canvas");
    m_fleetStatusLabel = new QLabel("0 selected", workspaceHeader);
    m_fleetStatusLabel->setObjectName("fleetStatus");
    m_selectAllButton = new QPushButton("All", workspaceHeader);
    m_selectNoneButton = new QPushButton("None", workspaceHeader);
    m_invertSelectionButton = new QPushButton("Invert", workspaceHeader);
    m_syncControlButton = new QPushButton("Sync off", workspaceHeader);
    bindI18n(m_selectAllButton, "All");
    bindI18n(m_selectNoneButton, "None");
    bindI18n(m_invertSelectionButton, "Invert");
    setMPhoneIcon(m_selectAllButton, QChar(0xf046));
    setMPhoneIcon(m_selectNoneButton, QChar(0xf096));
    setMPhoneIcon(m_invertSelectionButton, QChar(0xf074));
    setMPhoneIcon(m_syncControlButton, QChar(0xf0ec));
    m_syncControlButton->setObjectName("toggleButton");
    connect(m_selectAllButton, &QPushButton::clicked, this, &Dialog::selectAllDeviceCards);
    connect(m_selectNoneButton, &QPushButton::clicked, this, &Dialog::clearDeviceSelection);
    connect(m_invertSelectionButton, &QPushButton::clicked, this, &Dialog::invertDeviceSelection);
    connect(m_syncControlButton, &QPushButton::clicked, this, [this]() {
        m_syncControlEnabled = !m_syncControlEnabled;
        updateFleetSelectionUi();
    });
    workspaceHeaderLayout->addWidget(workspaceTitle, 0);
    workspaceHeaderLayout->addWidget(m_fleetStatusLabel, 0);
    workspaceHeaderLayout->addStretch(1);
    workspaceHeaderLayout->addWidget(m_selectAllButton, 0);
    workspaceHeaderLayout->addWidget(m_selectNoneButton, 0);
    workspaceHeaderLayout->addWidget(m_invertSelectionButton, 0);
    workspaceHeaderLayout->addWidget(m_syncControlButton, 0);

    m_mphoneContainer = new QWidget(workspace);
    m_mphoneContainer->setObjectName("DeviceGridViewport");
    m_mphoneContainer->setMinimumWidth(0);
    m_mphoneGrid = new QGridLayout(m_mphoneContainer);
    m_mphoneGrid->setContentsMargins(14, 14, 14, 14);
    m_mphoneGrid->setSpacing(12);

    m_mphoneScroll = new QScrollArea(workspace);
    m_mphoneScroll->setObjectName("DeviceCanvasScroll");
    m_mphoneScroll->setWidgetResizable(true);
    m_mphoneScroll->setFrameShape(QFrame::NoFrame);
    m_mphoneScroll->setWidget(m_mphoneContainer);
    m_mphoneScroll->setMinimumWidth(0);
    m_mphoneScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mphoneScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    workspaceLayout->addWidget(workspaceHeader, 0);
    workspaceLayout->addWidget(m_mphoneScroll, 1);
    mainSplitter->addWidget(workspace);

    auto *operationsPanel = new QFrame(mainSplitter);
    m_operationsPanel = operationsPanel;
    operationsPanel->setObjectName("OperationsPanel");
    auto *operationsLayout = new QVBoxLayout(operationsPanel);
    operationsLayout->setContentsMargins(10, 10, 10, 10);
    operationsLayout->setSpacing(10);
    auto *operationsTitle = new QLabel("Operations", operationsPanel);
    operationsTitle->setObjectName("PanelTitle");
    bindI18n(operationsTitle, "Operations");
    operationsLayout->addWidget(operationsTitle);
    auto *operationsTabs = new QTabWidget(operationsPanel);
    m_operationsTabs = operationsTabs;
    operationsTabs->setObjectName("OperationsTabs");
    operationsTabs->setDocumentMode(true);

    auto *selectionTab = new QWidget(operationsTabs);
    auto *selectionLayout = new QVBoxLayout(selectionTab);
    selectionLayout->setContentsMargins(8, 10, 8, 8);
    selectionLayout->setSpacing(8);
    auto *identityGrid = new QGridLayout();
    identityGrid->setHorizontalSpacing(8);
    identityGrid->setVerticalSpacing(8);
    identityGrid->addWidget(makePanelLabel("Name", selectionTab), 0, 0);
    identityGrid->addWidget(ui->userNameEdt, 0, 1);
    identityGrid->addWidget(ui->updateNameBtn, 0, 2);
    identityGrid->addWidget(makePanelLabel("Serial", selectionTab), 1, 0);
    identityGrid->addWidget(ui->serialBox, 1, 1, 1, 2);
    selectionLayout->addLayout(identityGrid);
    auto *selectionButtons = new QGridLayout();
    const QList<QPushButton *> selectionActionButtons = {
        ui->startServerBtn, ui->stopServerBtn, ui->stopAllServerBtn,
        ui->updateDevice, ui->getIPBtn, ui->startAdbdBtn,
        ui->installSndcpyBtn, ui->startAudioBtn, ui->stopAudioBtn,
    };
    selectionButtons->setHorizontalSpacing(8);
    selectionButtons->setVerticalSpacing(8);
    for (int i = 0; i < selectionActionButtons.size(); ++i) {
        selectionActionButtons.at(i)->setMinimumWidth(0);
        selectionActionButtons.at(i)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        selectionButtons->addWidget(selectionActionButtons.at(i), i / 2, i % 2);
    }
    selectionLayout->addLayout(selectionButtons);
    selectionLayout->addStretch(1);

    auto *streamTab = new QWidget(operationsTabs);
    auto *streamLayout = new QVBoxLayout(streamTab);
    streamLayout->setContentsMargins(8, 10, 8, 8);
    streamLayout->setSpacing(8);
    auto *streamGrid = new QGridLayout();
    streamGrid->setHorizontalSpacing(8);
    streamGrid->setVerticalSpacing(8);
    streamGrid->addWidget(makePanelLabel("Bitrate", streamTab), 0, 0);
    streamGrid->addWidget(ui->bitRateEdit, 0, 1);
    streamGrid->addWidget(ui->bitRateBox, 0, 2);
    streamGrid->addWidget(makePanelLabel("Size", streamTab), 1, 0);
    streamGrid->addWidget(ui->maxSizeBox, 1, 1, 1, 2);
    streamGrid->addWidget(makePanelLabel("Format", streamTab), 2, 0);
    streamGrid->addWidget(ui->formatBox, 2, 1, 1, 2);
    streamGrid->addWidget(makePanelLabel("Rotate", streamTab), 3, 0);
    streamGrid->addWidget(ui->lockOrientationBox, 3, 1, 1, 2);
    streamLayout->addLayout(streamGrid);
    auto *saveRow = new QHBoxLayout();
    saveRow->setSpacing(8);
    saveRow->addWidget(ui->recordPathEdt, 1);
    saveRow->addWidget(ui->selectRecordPathBtn, 0);
    streamLayout->addLayout(saveRow);
    auto *checkGrid = new QGridLayout();
    const QList<QCheckBox *> streamOptions = {
        ui->recordScreenCheck, ui->notDisplayCheck, ui->useReverseCheck,
        ui->fpsCheck, ui->alwaysTopCheck, ui->closeScreenCheck,
        ui->framelessCheck, ui->stayAwakeCheck, ui->showToolbar,
    };
    for (int i = 0; i < streamOptions.size(); ++i) {
        checkGrid->addWidget(streamOptions.at(i), i / 2, i % 2);
    }
    streamLayout->addLayout(checkGrid);
    streamLayout->addStretch(1);

    auto *networkTab = new QWidget(operationsTabs);
    auto *networkLayout = new QVBoxLayout(networkTab);
    networkLayout->setContentsMargins(8, 10, 8, 8);
    networkLayout->setSpacing(8);
    auto *networkRow = new QHBoxLayout();
    networkRow->setSpacing(6);
    networkRow->addWidget(ui->deviceIpEdt, 1);
    networkRow->addWidget(ui->label);
    networkRow->addWidget(ui->devicePortEdt, 0);
    networkLayout->addLayout(networkRow);
    auto *networkButtons = new QHBoxLayout();
    networkButtons->setSpacing(8);
    networkButtons->addWidget(ui->wirelessConnectBtn, 1);
    networkButtons->addWidget(ui->wirelessDisConnectBtn, 1);
    networkLayout->addLayout(networkButtons);
    networkLayout->addStretch(1);

    auto *scriptsTab = new QWidget(operationsTabs);
    auto *scriptsLayout = new QVBoxLayout(scriptsTab);
    scriptsLayout->setContentsMargins(8, 10, 8, 8);
    scriptsLayout->setSpacing(8);
    if (m_batchScriptGroup) {
        m_batchScriptGroup->setTitle("");
        scriptsLayout->addWidget(m_batchScriptGroup, 1);
    }

    operationsTabs->addTab(selectionTab, "Selection");
    operationsTabs->addTab(streamTab, "Stream");
    operationsTabs->addTab(networkTab, "Network");
    operationsTabs->addTab(scriptsTab, "Scripts");
    operationsLayout->addWidget(operationsTabs, 1);

    m_rightConfigScroll = new QScrollArea(this);
    m_rightConfigScroll->setObjectName("InspectorScroll");
    m_rightConfigScroll->setWidgetResizable(true);
    m_rightConfigScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_rightConfigScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_rightConfigScroll->setFrameShape(QFrame::NoFrame);
    m_rightConfigScroll->setWidget(operationsPanel);
    m_rightConfigScroll->setMinimumWidth(300);
    m_rightConfigScroll->setMaximumWidth(390);
    m_rightConfigScroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    ui->rightWidget->setParent(this);
    ui->rightWidget->setWindowFlags(Qt::Widget);
    ui->rightWidget->hide();
    mainSplitter->addWidget(m_rightConfigScroll);

    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setStretchFactor(2, 0);
    mainSplitter->setSizes(QList<int>() << 360 << qMax(720, availableGeometry.width() - 780) << 380);
    shellLayout->addWidget(mainSplitter, 1);

    auto *statusBar = new QFrame(this);
    statusBar->setObjectName("FleetStatusBar");
    statusBar->setFixedHeight(66);
    auto *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(14, 8, 14, 8);
    statusLayout->setSpacing(8);
    m_adbStatusLabel = new QLabel("ADB ready", statusBar);
    m_adbStatusLabel->setObjectName("StatusText");
    bindI18n(m_adbStatusLabel, "ADB ready");
    m_queueStatusLabel = new QLabel("Queue idle", statusBar);
    m_queueStatusLabel->setObjectName("StatusText");
    bindI18n(m_queueStatusLabel, "Queue idle");
    m_toggleLeftButton = new QPushButton("Devices", statusBar);
    m_toggleRightButton = new QPushButton("Inspector", statusBar);
    bindI18n(m_toggleLeftButton, "Devices");
    bindI18n(m_toggleRightButton, "Inspector");
    m_toggleLeftButton->setToolTip("Show or hide the device list");
    m_toggleRightButton->setToolTip("Show or hide settings and scripts");
    setMPhoneIcon(m_toggleLeftButton, QChar(0xf00a));
    setMPhoneIcon(m_toggleRightButton, QChar(0xf0c9));
    connect(m_toggleLeftButton, &QPushButton::clicked, this, &Dialog::toggleLeftPanel);
    connect(m_toggleRightButton, &QPushButton::clicked, this, &Dialog::toggleRightPanel);
    auto *dockConnectButton = new QPushButton("Connect", statusBar);
    auto *dockSelectButton = new QPushButton("Select", statusBar);
    auto *dockSyncButton = new QPushButton("Sync", statusBar);
    auto *dockHealthButton = new QPushButton("Health", statusBar);
    auto *dockRecoverButton = new QPushButton("Recover", statusBar);
    auto *dockApplyButton = new QPushButton("Run script", statusBar);
    auto *dockSettingsButton = new QPushButton("Settings", statusBar);
    dockConnectButton->setObjectName("dockPrimary");
    dockSelectButton->setObjectName("dockButton");
    dockSyncButton->setObjectName("dockButton");
    dockHealthButton->setObjectName("dockButton");
    dockRecoverButton->setObjectName("dockButton");
    dockApplyButton->setObjectName("dockButton");
    dockSettingsButton->setObjectName("dockButton");
    setMPhoneIcon(dockConnectButton, QChar(0xf067));
    setMPhoneIcon(dockSelectButton, QChar(0xf046));
    setMPhoneIcon(dockSyncButton, QChar(0xf0ec));
    setMPhoneIcon(dockHealthButton, QChar(0xf0e7));
    setMPhoneIcon(dockRecoverButton, QChar(0xf021));
    setMPhoneIcon(dockApplyButton, QChar(0xf1d9));
    setMPhoneIcon(dockSettingsButton, QChar(0xf013));
    connect(dockConnectButton, &QPushButton::clicked, this, &Dialog::startAllDeviceScreens);
    connect(dockSelectButton, &QPushButton::clicked, this, &Dialog::selectAllDeviceCards);
    connect(dockSyncButton, &QPushButton::clicked, this, [this]() {
        m_syncControlEnabled = !m_syncControlEnabled;
        updateFleetSelectionUi();
    });
    connect(dockHealthButton, &QPushButton::clicked, this, [this]() {
        const QStringList serials = selectedDeviceSerials().isEmpty() ? visibleDeviceSerials() : selectedDeviceSerials();
        if (serials.isEmpty()) {
            outLog("health check skipped: no target devices", false);
            return;
        }
        for (const QString &serial : serials) {
            runAdbShell(serial, QStringList() << "wm" << "size");
            runAdbShell(serial, QStringList() << "cmd" << "battery" << "get" << "level");
        }
    });
    connect(dockRecoverButton, &QPushButton::clicked, this, [this]() {
        int queued = 0;
        const QStringList serials = selectedDeviceSerials().isEmpty() ? visibleDeviceSerials() : selectedDeviceSerials();
        for (const QString &serial : serials) {
            QWidget *card = m_mphoneCards.value(serial, nullptr);
            if (card && card->property("state").toString() == "Streaming") {
                continue;
            }
            setMPhoneCard(serial, makeStatusCard(serial, "Queued", "Manual recovery queued"));
            if (!m_startQueue.contains(serial)) {
                m_startQueue.append(serial);
            }
            ++queued;
        }
        outLog(QString("recover queued: %1").arg(queued), false);
        pumpDeviceStartQueue();
    });
    connect(dockApplyButton, &QPushButton::clicked, this, &Dialog::runCurrentBatchScript);
    connect(dockSettingsButton, &QPushButton::clicked, this, &Dialog::toggleRightPanel);
    statusLayout->addWidget(dockConnectButton, 0);
    statusLayout->addWidget(dockSelectButton, 0);
    statusLayout->addWidget(dockSyncButton, 0);
    statusLayout->addWidget(dockHealthButton, 0);
    statusLayout->addWidget(dockRecoverButton, 0);
    statusLayout->addWidget(dockApplyButton, 0);
    statusLayout->addWidget(dockSettingsButton, 0);
    statusLayout->addSpacing(8);
    statusLayout->addWidget(m_adbStatusLabel, 0);
    statusLayout->addWidget(m_queueStatusLabel, 0);
    statusLayout->addStretch(1);
    statusLayout->addWidget(m_toggleLeftButton, 0);
    statusLayout->addWidget(m_toggleRightButton, 0);
    shellLayout->addWidget(statusBar, 0);

    applyMPhoneLanguage();
    relayoutMPhoneScreens();
}

QString Dialog::i18n(const QString &key) const
{
    return mphoneTranslate(m_languageCode, key);
}

void Dialog::applyMPhoneLanguage()
{
    const QList<QWidget *> widgets = findChildren<QWidget *>();
    for (QWidget *widget : widgets) {
        const QString key = widget->property("i18nKey").toString();
        if (key.isEmpty()) {
            continue;
        }
        const QString text = i18n(key);
        if (auto *button = qobject_cast<QPushButton *>(widget)) {
            button->setText(text);
        } else if (auto *checkBox = qobject_cast<QCheckBox *>(widget)) {
            checkBox->setText(text);
        } else if (auto *groupBox = qobject_cast<QGroupBox *>(widget)) {
            groupBox->setTitle(text);
        } else if (auto *label = qobject_cast<QLabel *>(widget)) {
            label->setText(text);
        }
    }

    if (m_operationsTabs) {
        m_operationsTabs->setTabText(0, i18n("Selection"));
        m_operationsTabs->setTabText(1, i18n("Stream"));
        m_operationsTabs->setTabText(2, i18n("Network"));
        m_operationsTabs->setTabText(3, i18n("Scripts"));
    }
    updateFleetSelectionUi();
    updateFleetMetrics();
}

void Dialog::localizeMPhoneUi()
{
    ui->useSingleModeCheck->setText("Compact mode");
    bindI18n(ui->useSingleModeCheck, "Compact mode");
    ui->useSingleModeCheck->hide();
    ui->simpleGroupBox->setTitle("Device Pool");
    bindI18n(ui->simpleGroupBox, "Device Pool");
    ui->wifiConnectBtn->setText("Wi-Fi");
    bindI18n(ui->wifiConnectBtn, "Wi-Fi");
    ui->usbConnectBtn->setText("Start USB");
    bindI18n(ui->usbConnectBtn, "Start USB");
    ui->autoUpdatecheckBox->setText("Auto refresh");
    bindI18n(ui->autoUpdatecheckBox, "Auto refresh");
    ui->label_10->setText("Devices");
    ui->adbGroupBox->setTitle("Diagnostics");
    bindI18n(ui->adbGroupBox, "Diagnostics");
    ui->label_7->setText("ADB");
    ui->adbCommandBtn->setText("Run");
    ui->stopAdbBtn->setText("Stop");
    ui->clearOut->setText("Clear");
    ui->rightWidget->setWindowTitle("Configuration");
    ui->configGroupBox->setTitle("Stream settings");
    bindI18n(ui->configGroupBox, "Stream settings");
    ui->usbGroupBox->setTitle("USB controls");
    bindI18n(ui->usbGroupBox, "USB controls");
    ui->wirelessGroupBox->setTitle("Wireless");
    bindI18n(ui->wirelessGroupBox, "Wireless");
    ui->label_3->setText("Bitrate");
    ui->label_4->setText("Size");
    ui->label_5->setText("Save path");
    ui->label_6->setText("Format");
    ui->label_8->setText("Orientation");
    ui->label_9->setText("Name");
    ui->label_2->setText("Serial");
    ui->startServerBtn->setText("Start");
    bindI18n(ui->startServerBtn, "Start");
    ui->stopServerBtn->setText("Stop");
    bindI18n(ui->stopServerBtn, "Stop");
    ui->stopAllServerBtn->setText("Stop all");
    bindI18n(ui->stopAllServerBtn, "Stop all");
    ui->updateDevice->setText("Refresh");
    bindI18n(ui->updateDevice, "Refresh");
    ui->getIPBtn->setText("Get IP");
    bindI18n(ui->getIPBtn, "Get IP");
    ui->startAdbdBtn->setText("Start ADBD");
    ui->wirelessConnectBtn->setText("Connect");
    bindI18n(ui->wirelessConnectBtn, "Connect");
    ui->wirelessDisConnectBtn->setText("Drop");
    bindI18n(ui->wirelessDisConnectBtn, "Drop");
    ui->installSndcpyBtn->setText("sndcpy");
    ui->startAudioBtn->setText("Audio on");
    bindI18n(ui->startAudioBtn, "Audio on");
    ui->stopAudioBtn->setText("Audio off");
    bindI18n(ui->stopAudioBtn, "Audio off");
    ui->selectRecordPathBtn->setText("...");
    ui->updateNameBtn->setText("Rename");
    bindI18n(ui->updateNameBtn, "Rename");
    ui->startAdbdBtn->setText("ADBD");
    ui->recordScreenCheck->setText("Record");
    bindI18n(ui->recordScreenCheck, "Record");
    ui->notDisplayCheck->setText("Background");
    bindI18n(ui->notDisplayCheck, "Background");
    ui->useReverseCheck->setText("Reverse");
    bindI18n(ui->useReverseCheck, "Reverse");
    ui->fpsCheck->setText("FPS");
    ui->alwaysTopCheck->setText("Top");
    bindI18n(ui->alwaysTopCheck, "Top");
    ui->closeScreenCheck->setText("Screen off");
    bindI18n(ui->closeScreenCheck, "Screen off");
    ui->framelessCheck->setText("Frameless");
    bindI18n(ui->framelessCheck, "Frameless");
    ui->stayAwakeCheck->setText("Awake");
    bindI18n(ui->stayAwakeCheck, "Awake");
    ui->showToolbar->setText("Toolbar");
    bindI18n(ui->showToolbar, "Toolbar");

    setMPhoneIcon(ui->wifiConnectBtn, QChar(0xf1eb));
    setMPhoneIcon(ui->usbConnectBtn, QChar(0xf287));
    setMPhoneIcon(ui->adbCommandBtn, QChar(0xf04b));
    setMPhoneIcon(ui->stopAdbBtn, QChar(0xf04d));
    setMPhoneIcon(ui->clearOut, QChar(0xf12d));
    setMPhoneIcon(ui->startServerBtn, QChar(0xf04b));
    setMPhoneIcon(ui->stopServerBtn, QChar(0xf04d));
    setMPhoneIcon(ui->stopAllServerBtn, QChar(0xf057), QColor("#ff9a9a"));
    setMPhoneIcon(ui->updateDevice, QChar(0xf021));
    setMPhoneIcon(ui->getIPBtn, QChar(0xf0ac));
    setMPhoneIcon(ui->startAdbdBtn, QChar(0xf120));
    setMPhoneIcon(ui->wirelessConnectBtn, QChar(0xf1eb));
    setMPhoneIcon(ui->wirelessDisConnectBtn, QChar(0xf00d), QColor("#ff9a9a"));
    setMPhoneIcon(ui->installSndcpyBtn, QChar(0xf019));
    setMPhoneIcon(ui->startAudioBtn, QChar(0xf028));
    setMPhoneIcon(ui->stopAudioBtn, QChar(0xf026));
    setMPhoneIcon(ui->selectRecordPathBtn, QChar(0xf07c));

    const QList<QPushButton *> compactButtons = {
        ui->selectRecordPathBtn,
        ui->updateNameBtn, ui->startServerBtn, ui->stopServerBtn,
        ui->stopAllServerBtn, ui->updateDevice, ui->getIPBtn, ui->startAdbdBtn,
        ui->installSndcpyBtn, ui->startAudioBtn, ui->stopAudioBtn,
        ui->wirelessConnectBtn, ui->wirelessDisConnectBtn,
    };
    for (QPushButton *button : compactButtons) {
        button->setMinimumWidth(0);
        button->setMaximumWidth(132);
    }
    ui->selectRecordPathBtn->setMaximumWidth(58);
    ui->bitRateEdit->setMaximumWidth(120);
    ui->bitRateBox->setMaximumWidth(86);
    ui->maxSizeBox->setMaximumWidth(132);
    ui->formatBox->setMaximumWidth(160);
    ui->lockOrientationBox->setMaximumWidth(150);
    ui->serialBox->setMaximumWidth(190);
    ui->devicePortEdt->setMaximumWidth(120);
    ui->deviceIpEdt->setMinimumWidth(210);
    ui->deviceIpEdt->setMaximumWidth(280);
    const QList<QCheckBox *> streamChecks = {
        ui->recordScreenCheck, ui->notDisplayCheck, ui->useReverseCheck, ui->fpsCheck,
        ui->alwaysTopCheck, ui->closeScreenCheck, ui->framelessCheck, ui->stayAwakeCheck,
        ui->showToolbar,
    };
    for (QCheckBox *checkBox : streamChecks) {
        checkBox->setMinimumWidth(0);
        checkBox->setMaximumWidth(108);
    }
    ui->adbCommandBtn->setText("");
    ui->stopAdbBtn->setText("");
    ui->clearOut->setText("");
    ui->adbCommandBtn->setToolTip("Run ADB command");
    ui->stopAdbBtn->setToolTip("Stop ADB command");
    ui->clearOut->setToolTip("Clear diagnostics");
    ui->adbCommandBtn->setFixedWidth(40);
    ui->stopAdbBtn->setFixedWidth(40);
    ui->clearOut->setFixedWidth(40);
}

void Dialog::rebuildMPhoneConfigPanel()
{
    auto *rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(10, 10, 10, 12);
    rightLayout->setSpacing(10);
    replaceWidgetLayout(ui->rightWidget, rightLayout);

    auto stylePanel = [](QGroupBox *groupBox) {
        groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        groupBox->setMinimumWidth(0);
        groupBox->setMaximumWidth(QWIDGETSIZE_MAX);
    };

    stylePanel(ui->configGroupBox);
    stylePanel(ui->usbGroupBox);
    stylePanel(ui->wirelessGroupBox);

    auto *streamLayout = new QVBoxLayout();
    streamLayout->setContentsMargins(12, 18, 12, 12);
    streamLayout->setSpacing(8);
    replaceWidgetLayout(ui->configGroupBox, streamLayout);

    auto *qualityGrid = new QGridLayout();
    qualityGrid->setContentsMargins(0, 0, 0, 0);
    qualityGrid->setHorizontalSpacing(8);
    qualityGrid->setVerticalSpacing(8);
    qualityGrid->addWidget(makePanelLabel("Bitrate", ui->configGroupBox), 0, 0);
    qualityGrid->addWidget(ui->bitRateEdit, 0, 1);
    qualityGrid->addWidget(ui->bitRateBox, 0, 2);
    qualityGrid->addWidget(makePanelLabel("Size", ui->configGroupBox), 1, 0);
    qualityGrid->addWidget(ui->maxSizeBox, 1, 1, 1, 2);
    qualityGrid->addWidget(makePanelLabel("Format", ui->configGroupBox), 2, 0);
    qualityGrid->addWidget(ui->formatBox, 2, 1, 1, 2);
    qualityGrid->addWidget(makePanelLabel("Rotation", ui->configGroupBox), 3, 0);
    qualityGrid->addWidget(ui->lockOrientationBox, 3, 1, 1, 2);
    qualityGrid->setColumnStretch(1, 1);
    streamLayout->addLayout(qualityGrid);

    auto *pathRow = new QHBoxLayout();
    pathRow->setContentsMargins(0, 0, 0, 0);
    pathRow->setSpacing(8);
    pathRow->addWidget(makePanelLabel("Save path", ui->configGroupBox));
    pathRow->addWidget(ui->recordPathEdt, 1);
    pathRow->addWidget(ui->selectRecordPathBtn, 0);
    streamLayout->addLayout(pathRow);

    auto *scriptRow = new QHBoxLayout();
    scriptRow->setContentsMargins(0, 0, 0, 0);
    scriptRow->setSpacing(8);

    auto *optionGrid = new QGridLayout();
    optionGrid->setContentsMargins(0, 2, 0, 0);
    optionGrid->setHorizontalSpacing(8);
    optionGrid->setVerticalSpacing(6);
    const QList<QCheckBox *> options = {
        ui->recordScreenCheck, ui->notDisplayCheck, ui->useReverseCheck,
        ui->fpsCheck, ui->alwaysTopCheck, ui->closeScreenCheck,
        ui->framelessCheck, ui->stayAwakeCheck, ui->showToolbar,
    };
    for (int i = 0; i < options.size(); ++i) {
        optionGrid->addWidget(options.at(i), i / 2, i % 2);
    }
    streamLayout->addLayout(optionGrid);

    auto *usbLayout = new QVBoxLayout();
    usbLayout->setContentsMargins(12, 18, 12, 12);
    usbLayout->setSpacing(8);
    replaceWidgetLayout(ui->usbGroupBox, usbLayout);

    auto *nameRow = new QHBoxLayout();
    nameRow->setContentsMargins(0, 0, 0, 0);
    nameRow->setSpacing(8);
    nameRow->addWidget(makePanelLabel("Name", ui->usbGroupBox));
    nameRow->addWidget(ui->userNameEdt, 1);
    nameRow->addWidget(ui->updateNameBtn, 0);
    usbLayout->addLayout(nameRow);

    auto *serialRow = new QHBoxLayout();
    serialRow->setContentsMargins(0, 0, 0, 0);
    serialRow->setSpacing(8);
    serialRow->addWidget(makePanelLabel("Serial", ui->usbGroupBox));
    serialRow->addWidget(ui->serialBox, 1);
    usbLayout->addLayout(serialRow);

    auto *usbButtonGrid = new QGridLayout();
    usbButtonGrid->setContentsMargins(0, 0, 0, 0);
    usbButtonGrid->setHorizontalSpacing(8);
    usbButtonGrid->setVerticalSpacing(8);
    const QList<QPushButton *> usbButtons = {
        ui->startServerBtn, ui->stopServerBtn, ui->stopAllServerBtn,
        ui->updateDevice, ui->getIPBtn, ui->startAdbdBtn,
        ui->installSndcpyBtn, ui->startAudioBtn, ui->stopAudioBtn,
    };
    for (int i = 0; i < usbButtons.size(); ++i) {
        usbButtonGrid->addWidget(usbButtons.at(i), i / 3, i % 3);
    }
    usbLayout->addLayout(usbButtonGrid);

    auto *wirelessLayout = new QVBoxLayout();
    wirelessLayout->setContentsMargins(12, 18, 12, 12);
    wirelessLayout->setSpacing(8);
    replaceWidgetLayout(ui->wirelessGroupBox, wirelessLayout);

    auto *addressRow = new QHBoxLayout();
    addressRow->setContentsMargins(0, 0, 0, 0);
    addressRow->setSpacing(6);
    addressRow->addWidget(ui->deviceIpEdt, 1);
    addressRow->addWidget(ui->label, 0);
    addressRow->addWidget(ui->devicePortEdt, 0);
    wirelessLayout->addLayout(addressRow);

    auto *wirelessButtons = new QHBoxLayout();
    wirelessButtons->setContentsMargins(0, 0, 0, 0);
    wirelessButtons->setSpacing(8);
    wirelessButtons->addWidget(ui->wirelessConnectBtn, 1);
    wirelessButtons->addWidget(ui->wirelessDisConnectBtn, 1);
    wirelessLayout->addLayout(wirelessButtons);

    const QList<QWidget *> flexibleFields = {
        ui->recordPathEdt, ui->userNameEdt, ui->serialBox, ui->deviceIpEdt,
        ui->bitRateEdit, ui->bitRateBox, ui->maxSizeBox, ui->formatBox, ui->lockOrientationBox,
    };
    for (QWidget *field : flexibleFields) {
        field->setMinimumWidth(0);
        field->setMaximumWidth(QWIDGETSIZE_MAX);
        field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    ui->bitRateBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->devicePortEdt->setMinimumWidth(72);
    ui->devicePortEdt->setMaximumWidth(86);
    ui->selectRecordPathBtn->setFixedWidth(42);

    const QList<QPushButton *> drawerButtons = {
        ui->updateNameBtn, ui->startServerBtn, ui->stopServerBtn, ui->stopAllServerBtn,
        ui->updateDevice, ui->getIPBtn, ui->startAdbdBtn, ui->installSndcpyBtn,
        ui->startAudioBtn, ui->stopAudioBtn, ui->wirelessConnectBtn, ui->wirelessDisConnectBtn,
    };
    for (QPushButton *button : drawerButtons) {
        button->setMinimumWidth(0);
        button->setMaximumWidth(QWIDGETSIZE_MAX);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    rightLayout->addWidget(ui->configGroupBox);
    rightLayout->addWidget(ui->usbGroupBox);
    rightLayout->addWidget(ui->wirelessGroupBox);
    rightLayout->addStretch(1);
}

void Dialog::initBatchScriptPanel()
{
    if (m_batchScriptGroup) {
        return;
    }

    m_batchScriptGroup = new QGroupBox("Batch scripts", ui->rightWidget);
    auto *layout = new QVBoxLayout(m_batchScriptGroup);
    layout->setContentsMargins(12, 18, 12, 12);
    layout->setSpacing(8);

    auto *topRow = new QHBoxLayout();
    topRow->setSpacing(6);
    m_batchScriptSelector = new QComboBox(m_batchScriptGroup);
    m_batchScriptSelector->setEditable(true);
    m_batchScriptSelector->setInsertPolicy(QComboBox::NoInsert);
    m_batchScriptSelector->setMinimumWidth(0);
    m_batchScriptSelector->setToolTip("Saved script name");
    m_newScriptButton = new QPushButton(m_batchScriptGroup);
    m_saveScriptButton = new QPushButton(m_batchScriptGroup);
    m_deleteScriptButton = new QPushButton(m_batchScriptGroup);
    m_runScriptButton = new QPushButton(m_batchScriptGroup);

    const QList<QPair<QPushButton *, QChar>> buttons = {
        {m_newScriptButton, QChar(0xf067)},
        {m_saveScriptButton, QChar(0xf0c7)},
        {m_deleteScriptButton, QChar(0xf1f8)},
        {m_runScriptButton, QChar(0xf04b)},
    };
    for (const auto &item : buttons) {
        item.first->setFixedSize(36, 32);
        item.first->setText("");
        setMPhoneIcon(item.first, item.second);
    }
    m_newScriptButton->setToolTip("New script");
    m_saveScriptButton->setToolTip("Save script");
    m_deleteScriptButton->setToolTip("Delete script");
    m_runScriptButton->setToolTip("Run script on chosen target scope");
    m_runScriptButton->setObjectName("primaryButton");

    topRow->addWidget(m_batchScriptSelector, 1);
    topRow->addWidget(m_newScriptButton);
    topRow->addWidget(m_saveScriptButton);
    topRow->addWidget(m_deleteScriptButton);
    topRow->addWidget(m_runScriptButton);

    m_scriptTargetBox = new QComboBox(m_batchScriptGroup);
    m_scriptTargetBox->setObjectName("ScriptTargetScope");
    m_scriptTargetBox->addItem("Selected devices", "selected");
    m_scriptTargetBox->addItem("Visible devices", "visible");
    m_scriptTargetBox->addItem("All online devices", "online");
    m_scriptTargetBox->setToolTip("Target scope for batch script execution");

    m_batchScriptEditor = new QPlainTextEdit(m_batchScriptGroup);
    m_batchScriptEditor->setMinimumHeight(180);
    m_batchScriptEditor->setPlaceholderText(
        "# Commands run on every online device\n"
        "key HOME\n"
        "sleep 500\n"
        "tap 540 1600\n"
        "text hello-mphone\n"
        "screencap /sdcard/mphone-shot.png\n"
        "shell input keyevent BACK\n");

    auto *hint = new QLabel("Commands: key, tap, swipe, text, shell, launch, stop, clear, install, uninstall, grant, push, pull, screencap, sleep", m_batchScriptGroup);
    hint->setWordWrap(true);
    hint->setObjectName("mutedHint");

    layout->addLayout(topRow);
    layout->addWidget(m_scriptTargetBox);
    layout->addWidget(m_batchScriptEditor);
    layout->addWidget(hint);
    if (auto *rightLayout = qobject_cast<QVBoxLayout *>(ui->rightWidget->layout())) {
        const int insertIndex = qMax(0, rightLayout->count() - 1);
        rightLayout->insertWidget(insertIndex, m_batchScriptGroup);
    }

    connect(m_batchScriptSelector, &QComboBox::currentTextChanged, this, &Dialog::loadBatchScript);
    connect(m_newScriptButton, &QPushButton::clicked, this, [this]() {
        m_batchScriptSelector->setEditText("new-script");
        m_batchScriptEditor->clear();
        m_batchScriptEditor->setFocus();
    });
    connect(m_saveScriptButton, &QPushButton::clicked, this, &Dialog::saveCurrentBatchScript);
    connect(m_deleteScriptButton, &QPushButton::clicked, this, &Dialog::deleteCurrentBatchScript);
    connect(m_runScriptButton, &QPushButton::clicked, this, &Dialog::runCurrentBatchScript);

    loadBatchScriptList();
    if (m_batchScriptSelector->count() == 0) {
        m_batchScriptSelector->setEditText("daily-check");
    }
}

QString Dialog::batchScriptDir() const
{
    QString root = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (root.isEmpty()) {
        root = QDir::homePath() + "/.config/MPhone";
    }
    return root + "/scripts";
}

void Dialog::loadBatchScriptList()
{
    if (!m_batchScriptSelector) {
        return;
    }

    QDir dir(batchScriptDir());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString current = m_batchScriptSelector->currentText();
    m_batchScriptSelector->blockSignals(true);
    m_batchScriptSelector->clear();
    const QFileInfoList files = dir.entryInfoList(QStringList() << "*.pbs", QDir::Files, QDir::Name);
    for (const QFileInfo &file : files) {
        m_batchScriptSelector->addItem(file.completeBaseName());
    }
    if (!current.isEmpty()) {
        m_batchScriptSelector->setEditText(current);
    }
    m_batchScriptSelector->blockSignals(false);
}

void Dialog::loadBatchScript(const QString &name)
{
    if (!m_batchScriptEditor || name.trimmed().isEmpty()) {
        return;
    }

    QFile file(batchScriptDir() + "/" + name.trimmed() + ".pbs");
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    m_batchScriptEditor->setPlainText(QString::fromUtf8(file.readAll()));
}

void Dialog::saveCurrentBatchScript()
{
    const QString name = m_batchScriptSelector ? m_batchScriptSelector->currentText().trimmed() : QString();
    if (name.isEmpty() || !m_batchScriptEditor) {
        outLog("script save failed: missing name", false);
        return;
    }

    QDir dir(batchScriptDir());
    if (!dir.exists() && !dir.mkpath(".")) {
        outLog("script save failed: cannot create script folder", false);
        return;
    }

    QFile file(dir.filePath(name + ".pbs"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        outLog("script save failed: " + file.errorString(), false);
        return;
    }
    file.write(m_batchScriptEditor->toPlainText().toUtf8());
    file.close();
    loadBatchScriptList();
    m_batchScriptSelector->setEditText(name);
    outLog("script saved: " + name, false);
}

void Dialog::deleteCurrentBatchScript()
{
    const QString name = m_batchScriptSelector ? m_batchScriptSelector->currentText().trimmed() : QString();
    if (name.isEmpty()) {
        return;
    }
    QFile::remove(batchScriptDir() + "/" + name + ".pbs");
    m_batchScriptEditor->clear();
    loadBatchScriptList();
    outLog("script deleted: " + name, false);
}

QStringList Dialog::selectedBatchScriptDevices() const
{
    const QString scope = m_scriptTargetBox ? m_scriptTargetBox->currentData().toString() : QString("selected");
    if (scope == "visible") {
        return visibleDeviceSerials();
    }

    if (scope == "selected") {
        const QStringList selected = selectedDeviceSerials();
        if (!selected.isEmpty()) {
            return selected;
        }
        return QStringList();
    }

    QStringList serials;
    for (int i = 0; i < ui->serialBox->count(); ++i) {
        const QString serial = ui->serialBox->itemText(i).trimmed();
        if (!serial.isEmpty() && !serials.contains(serial)) {
            serials.append(serial);
        }
    }
    return serials;
}

void Dialog::runCurrentBatchScript()
{
    if (!m_batchScriptEditor) {
        return;
    }

    const QStringList serials = selectedBatchScriptDevices();
    if (serials.isEmpty()) {
        outLog("script run skipped: no online devices", false);
        return;
    }

    const QStringList lines = m_batchScriptEditor->toPlainText().split('\n');
    outLog(QString("script run: %1 devices").arg(serials.size()), false);
    for (const QString &rawLine : lines) {
        QString line = rawLine.trimmed();
        if (line.isEmpty() || line.startsWith("#") || line.startsWith("//")) {
            continue;
        }

        if (line.startsWith("sleep ", Qt::CaseInsensitive)) {
            bool ok = false;
            const int ms = line.section(' ', 1, 1).toInt(&ok);
            delayMs(ok ? ms : 300);
            continue;
        }

        for (const QString &serial : serials) {
            runScriptLineForDevice(serial, line);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }
    }
    outLog("script complete", false);
}

void Dialog::runScriptLineForDevice(const QString &serial, const QString &line)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
#else
    const QStringList parts = line.split(' ', QString::SkipEmptyParts);
#endif
    if (parts.isEmpty()) {
        return;
    }

    const QString command = parts.first().toLower();
    if (command == "key" && parts.size() >= 2) {
        const QString key = parts.at(1).toUpper();
        const QMap<QString, QString> keyMap = {
            {"BACK", "4"},
            {"HOME", "3"},
            {"RECENT", "187"},
            {"APP_SWITCH", "187"},
            {"POWER", "26"},
            {"VOLUP", "24"},
            {"VOLUME_UP", "24"},
            {"VOLDOWN", "25"},
            {"VOLUME_DOWN", "25"},
            {"WAKE", "224"},
            {"SLEEP", "223"},
        };
        runAdbShell(serial, QStringList() << "input" << "keyevent" << keyMap.value(key, key));
    } else if (command == "tap" && parts.size() >= 3) {
        runAdbShell(serial, QStringList() << "input" << "tap" << parts.at(1) << parts.at(2));
    } else if (command == "swipe" && parts.size() >= 5) {
        QStringList args = QStringList() << "input" << "swipe" << parts.at(1) << parts.at(2) << parts.at(3) << parts.at(4);
        if (parts.size() >= 6) {
            args << parts.at(5);
        }
        runAdbShell(serial, args);
    } else if (command == "text" && parts.size() >= 2) {
        QString text = line.mid(line.indexOf(' ') + 1);
        text.replace(" ", "%s");
        runAdbShell(serial, QStringList() << "input" << "text" << text);
    } else if (command == "shell" && parts.size() >= 2) {
        runAdbShell(serial, parts.mid(1));
    } else if (command == "launch" && parts.size() >= 2) {
        runAdbShell(serial, QStringList() << "monkey" << "-p" << parts.at(1) << "-c" << "android.intent.category.LAUNCHER" << "1");
    } else if (command == "stop" && parts.size() >= 2) {
        runAdbShell(serial, QStringList() << "am" << "force-stop" << parts.at(1));
    } else if (command == "clear" && parts.size() >= 2) {
        runAdbShell(serial, QStringList() << "pm" << "clear" << parts.at(1));
    } else if (command == "install" && parts.size() >= 2) {
        runAdbCommand(serial, QStringList() << "install" << "-r" << parts.mid(1).join(" "), 120000);
    } else if (command == "uninstall" && parts.size() >= 2) {
        runAdbCommand(serial, QStringList() << "uninstall" << parts.at(1), 60000);
    } else if (command == "grant" && parts.size() >= 3) {
        runAdbShell(serial, QStringList() << "pm" << "grant" << parts.at(1) << parts.at(2));
    } else if (command == "revoke" && parts.size() >= 3) {
        runAdbShell(serial, QStringList() << "pm" << "revoke" << parts.at(1) << parts.at(2));
    } else if (command == "push" && parts.size() >= 3) {
        runAdbCommand(serial, QStringList() << "push" << parts.at(1) << parts.at(2), 120000);
    } else if (command == "pull" && parts.size() >= 3) {
        runAdbCommand(serial, QStringList() << "pull" << parts.at(1) << parts.at(2), 120000);
    } else if (command == "screencap") {
        const QString remotePath = parts.size() >= 2 ? parts.at(1) : QString("/sdcard/mphone-shot.png");
        runAdbShell(serial, QStringList() << "screencap" << "-p" << remotePath);
    } else if (command == "activity") {
        runAdbShell(serial, QStringList() << "dumpsys" << "window" << "windows");
    } else if (command == "battery") {
        runAdbShell(serial, QStringList() << "cmd" << "battery" << "get" << "level");
    } else if (command == "size") {
        runAdbShell(serial, QStringList() << "wm" << "size");
    } else {
        outLog("script unknown command: " + line, false);
    }
}

bool Dialog::runAdbCommand(const QString &serial, const QStringList &adbArgs, int timeoutMs)
{
    QString adbPath = QString::fromLocal8Bit(qgetenv("MPHONE_ADB_PATH"));
    if (adbPath.isEmpty()) {
        adbPath = Config::getInstance().getAdbPath();
    }
    if (adbPath.isEmpty()) {
        adbPath = "adb";
    }

    QStringList args;
    if (!serial.isEmpty()) {
        args << "-s" << serial;
    }
    args << adbArgs;

    QProcess process;
    outLog(QString("adb: %1 -> %2").arg(serial, adbArgs.join(' ')), false);
    process.start(adbPath, args);
    if (!process.waitForStarted(3000)) {
        outLog("adb start failed: " + serial, false);
        return false;
    }
    if (!process.waitForFinished(timeoutMs)) {
        const QString partial = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
        process.kill();
        process.waitForFinished(1000);
        outLog(QString("adb timeout: %1 -> %2").arg(serial, adbArgs.join(' ')), false);
        if (!partial.isEmpty()) {
            outLog(serial + ": " + partial.left(3000), false);
        }
        return false;
    }
    const QString stdoutText = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    const QString stderrText = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    if (process.exitCode() != 0) {
        outLog("adb failed: " + serial + " " + stderrText.left(2000), false);
        return false;
    }
    const QString output = stdoutText.isEmpty() ? stderrText : stdoutText;
    if (!output.isEmpty()) {
        const QString suffix = output.size() > 4000 ? "\n... output truncated" : "";
        outLog(serial + ": " + output.left(4000) + suffix, false);
    } else {
        outLog("adb done: " + serial, false);
    }
    return true;
}

bool Dialog::runAdbShell(const QString &serial, const QStringList &shellArgs)
{
    QString adbPath = QString::fromLocal8Bit(qgetenv("MPHONE_ADB_PATH"));
    if (adbPath.isEmpty()) {
        adbPath = Config::getInstance().getAdbPath();
    }
    if (adbPath.isEmpty()) {
        adbPath = "adb";
    }

    QStringList args;
    if (!serial.isEmpty()) {
        args << "-s" << serial;
    }
    args << "shell";
    args << shellArgs;

    QProcess process;
    outLog(QString("adb shell: %1 -> %2").arg(serial, shellArgs.join(' ')), false);
    process.start(adbPath, args);
    if (!process.waitForStarted(3000)) {
        outLog("adb start failed: " + serial, false);
        return false;
    }
    if (!process.waitForFinished(15000)) {
        const QString partial = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
        process.kill();
        process.waitForFinished(1000);
        outLog(QString("adb timeout: %1 -> %2").arg(serial, shellArgs.join(' ')), false);
        if (!partial.isEmpty()) {
            outLog(serial + ": " + partial.left(3000), false);
        }
        return false;
    }
    if (process.exitCode() != 0) {
        outLog("adb failed: " + serial + " " + QString::fromLocal8Bit(process.readAllStandardError()).trimmed().left(2000), false);
        return false;
    }
    const QString output = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    if (!output.isEmpty()) {
        const QString suffix = output.size() > 4000 ? "\n... output truncated" : "";
        outLog(serial + ": " + output.left(4000) + suffix, false);
    } else {
        outLog("adb done: " + serial, false);
    }
    return true;
}

bool Dialog::hasAndroidService(const QString &serial, const QString &serviceName)
{
    QString adbPath = QString::fromLocal8Bit(qgetenv("MPHONE_ADB_PATH"));
    if (adbPath.isEmpty()) {
        adbPath = Config::getInstance().getAdbPath();
    }
    if (adbPath.isEmpty()) {
        adbPath = "adb";
    }

    QProcess process;
    QStringList args;
    args << "-s" << serial << "shell" << "service" << "check" << serviceName;
    process.start(adbPath, args);
    if (!process.waitForFinished(2500)) {
        process.kill();
        process.waitForFinished(500);
        outLog(QString("service check timeout: %1 -> %2").arg(serial, serviceName), false);
        return true;
    }

    const QString output = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    const QString error = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    const QString combined = output + "\n" + error;
    return !combined.contains("not found", Qt::CaseInsensitive) &&
           !combined.contains("Can't find service", Qt::CaseInsensitive) &&
           !combined.contains("ServiceNotFound", Qt::CaseInsensitive);
}

QString Dialog::mphoneAgentApkPath() const
{
    const QString overridePath = QString::fromLocal8Bit(qgetenv("MPHONE_AGENT_APK"));
    if (!overridePath.isEmpty() && QFileInfo::exists(overridePath)) {
        return overridePath;
    }

    const QString bundledPath = QDir(QCoreApplication::applicationDirPath()).filePath("mphone-agent.apk");
    if (QFileInfo::exists(bundledPath)) {
        return bundledPath;
    }

    const QString sourceTreePath = QDir(QCoreApplication::applicationDirPath())
                                       .absoluteFilePath("../../../../../agent/android/build/mphone-agent.apk");
    if (QFileInfo::exists(sourceTreePath)) {
        return QFileInfo(sourceTreePath).absoluteFilePath();
    }

    return bundledPath;
}

void Dialog::ensureMPhoneAgent(const QString &serial)
{
    const QString cleanSerial = serial.trimmed();
    if (cleanSerial.isEmpty() || m_agentCheckedDevices.contains(cleanSerial)) {
        return;
    }
    m_agentCheckedDevices.insert(cleanSerial);

    outLog("agent check: " + cleanSerial + " package com.mphone.agent", false);
    const bool installed = runAdbCommand(cleanSerial, QStringList() << "shell" << "pm" << "path" << "com.mphone.agent", 8000);
    if (installed) {
        outLog("MPhone Agent already installed: " + cleanSerial, false);
        startMPhoneAgent(cleanSerial);
        applyMPhoneAgentHardening(cleanSerial);
        return;
    }

    outLog("MPhone Agent missing, installing: " + cleanSerial, false);
    if (installMPhoneAgent(cleanSerial)) {
        startMPhoneAgent(cleanSerial);
        applyMPhoneAgentHardening(cleanSerial);
    }
}

bool Dialog::installMPhoneAgent(const QString &serial)
{
    if (serial.trimmed().isEmpty()) {
        return false;
    }

    const QString apkPath = mphoneAgentApkPath();
    if (!QFileInfo::exists(apkPath)) {
        outLog("MPhone Agent APK not found: " + apkPath, false);
        outLog("Build it with: ./agent/android/build-agent.sh", false);
        return false;
    }

    outLog("install MPhone Agent: " + serial, false);
    if (!runAdbCommand(serial, QStringList() << "install" << "-r" << "-g" << apkPath, 60000)) {
        return false;
    }

    runAdbShell(serial, QStringList() << "cmd" << "deviceidle" << "whitelist" << "+com.mphone.agent");
    runAdbShell(serial, QStringList() << "appops" << "set" << "com.mphone.agent" << "RUN_ANY_IN_BACKGROUND" << "allow");
    runAdbShell(serial, QStringList() << "appops" << "set" << "com.mphone.agent" << "WAKE_LOCK" << "allow");
    return true;
}

bool Dialog::startMPhoneAgent(const QString &serial)
{
    if (serial.trimmed().isEmpty()) {
        return false;
    }

    outLog("start MPhone Agent: " + serial, false);
    if (!runAdbShell(serial, QStringList() << "am" << "start-foreground-service" << "-n"
                                           << "com.mphone.agent/.MPhoneAgentService")) {
        return runAdbShell(serial, QStringList() << "am" << "startservice" << "-n"
                                                << "com.mphone.agent/.MPhoneAgentService");
    }
    return true;
}

void Dialog::applyMPhoneAgentHardening(const QString &serial)
{
    if (serial.trimmed().isEmpty()) {
        return;
    }

    outLog("apply phonebox hardening: " + serial, false);

    const QList<QStringList> shellCommands = {
        // Keep screen/device awake while USB/AC/wireless charging is present.
        QStringList() << "svc" << "power" << "stayon" << "true",
        QStringList() << "settings" << "put" << "global" << "stay_on_while_plugged_in" << "7",

        // Keep ADB enabled/persistent on ROMs that expose these properties/settings.
        QStringList() << "settings" << "put" << "global" << "adb_enabled" << "1",
        QStringList() << "setprop" << "persist.sys.usb.config" << "adb",
        QStringList() << "setprop" << "persist.service.adb.enable" << "1",
        QStringList() << "setprop" << "service.adb.tcp.port" << "5555",

        // Reduce background killing / idle modes for phonebox usage.
        QStringList() << "cmd" << "deviceidle" << "disable",
        QStringList() << "dumpsys" << "deviceidle" << "disable",
        QStringList() << "cmd" << "deviceidle" << "whitelist" << "+com.mphone.agent",
        QStringList() << "appops" << "set" << "com.mphone.agent" << "RUN_ANY_IN_BACKGROUND" << "allow",
        QStringList() << "appops" << "set" << "com.mphone.agent" << "WAKE_LOCK" << "allow",
        QStringList() << "appops" << "set" << "com.mphone.agent" << "START_FOREGROUND" << "allow",

        // Power menu / power-key knobs used by several AOSP/OEM/modded ROMs.
        QStringList() << "settings" << "put" << "global" << "power_button_long_press" << "0",
        QStringList() << "settings" << "put" << "secure" << "power_button_long_press" << "0",
        QStringList() << "settings" << "put" << "global" << "long_press_power_button_behavior" << "0",
        QStringList() << "settings" << "put" << "secure" << "long_press_power_button_behavior" << "0",
        QStringList() << "settings" << "put" << "global" << "power_menu_locked_show_content" << "0",
        QStringList() << "settings" << "put" << "secure" << "lockscreen_power_button_instantly_locks" << "0",
    };

    for (const QStringList &command : shellCommands) {
        runAdbShell(serial, command);
    }

    startMPhoneAgent(serial);
}

void Dialog::showMPhoneAgentStatus(const QString &serial)
{
    runAdbShell(serial, QStringList() << "pm" << "path" << "com.mphone.agent");
    runAdbShell(serial, QStringList() << "pidof" << "com.mphone.agent");
    runAdbShell(serial, QStringList() << "settings" << "get" << "global" << "stay_on_while_plugged_in");
    runAdbShell(serial, QStringList() << "settings" << "get" << "global" << "power_button_long_press");
    runAdbShell(serial, QStringList() << "settings" << "get" << "secure" << "power_button_long_press");
    runAdbShell(serial, QStringList() << "dumpsys" << "power");
}

void Dialog::applyMPhoneStyle()
{
    qApp->setStyleSheet(R"(
        QWidget {
            font-family: "SF Pro Text", "Segoe UI", "Inter", "Noto Sans", sans-serif;
            font-size: 13px;
            color: #eef3f8;
            background: #0d1117;
        }
        QWidget#Widget, QMainWindow {
            background: #0d1117;
        }
        QWidget#leftWidget {
            background: #131a22;
            border-right: 1px solid #26313d;
        }
        QWidget#DevicePoolPanel {
            background: #131a22;
            border-right: 1px solid #26313d;
        }
        QWidget#NavigationShell {
            background: #10161d;
            border-right: 1px solid #26313d;
        }
        QFrame#NavRail {
            background: #0f151c;
            border-right: 1px solid #26313d;
        }
        QPushButton#NavRailButton {
            background: transparent;
            border: 1px solid transparent;
            border-radius: 6px;
            color: #a7b2c0;
            font-size: 13px;
            font-weight: 700;
            padding: 7px 12px;
            text-align: left;
        }
        QPushButton#NavRailButton:hover {
            background: #18212b;
            border-color: #354454;
            color: #eef3f8;
        }
        QPushButton#NavRailButton[active="true"] {
            background: #123030;
            border-color: #13a89e;
            color: #eef3f8;
        }
        QFrame#TopBar {
            background: #131a22;
            border-bottom: 1px solid #26313d;
        }
        QLabel#AppBrand {
            color: #eef3f8;
            font-size: 16px;
            font-weight: 800;
        }
        QLabel#AppSubtitle {
            color: #a7b2c0;
            font-size: 12px;
            font-weight: 600;
        }
        QLabel#MetricChip {
            color: #eef3f8;
            background: #0f151c;
            border: 1px solid #26313d;
            border-radius: 6px;
            padding: 4px 9px;
            font-weight: 700;
        }
        QLabel#MetricChip[active="true"] {
            color: #ffd0d4;
            background: #2a151b;
            border-color: #d75f6a;
        }
        QWidget#FleetWorkspace, QWidget#commandDeck {
            background: #0d1117;
        }
        QWidget#OperationsPanel {
            background: #131a22;
        }
        QWidget#OperationsPanel QPushButton {
            min-width: 0px;
            padding: 7px 8px;
        }
        QWidget#OperationsPanel QLineEdit,
        QWidget#OperationsPanel QTextEdit,
        QWidget#OperationsPanel QPlainTextEdit,
        QWidget#OperationsPanel QComboBox {
            min-width: 0px;
        }
        QScrollArea#InspectorScroll {
            background: #131a22;
            border-left: 1px solid #26313d;
        }
        QFrame#WorkspaceHeader, QWidget#commandToolbar {
            background: #131a22;
            border: 1px solid #26313d;
            border-radius: 6px;
        }
        QLabel#WorkspaceTitle {
            color: #eef3f8;
            font-size: 14px;
            font-weight: 800;
        }
        QLabel#PanelTitle {
            color: #eef3f8;
            font-size: 14px;
            font-weight: 800;
        }
        QLabel#PanelMeta {
            color: #a7b2c0;
            background: #0f151c;
            border: 1px solid #26313d;
            border-radius: 6px;
            padding: 7px 9px;
            font-size: 12px;
            font-weight: 700;
        }
        QTabWidget#OperationsTabs::pane {
            border: 1px solid #26313d;
            border-radius: 6px;
            background: #131a22;
            top: -1px;
        }
        QTabBar::tab {
            background: #0f151c;
            color: #a7b2c0;
            border: 1px solid #26313d;
            border-bottom: 0;
            padding: 7px 10px;
            min-width: 48px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            font-weight: 700;
        }
        QTabBar::tab:selected {
            background: #18212b;
            color: #eef3f8;
            border-color: #13a89e;
        }
        QFrame#FleetStatusBar {
            background: #131a22;
            border-top: 1px solid #26313d;
        }
        QLabel#StatusText {
            color: #a7b2c0;
            font-size: 12px;
            font-weight: 600;
        }
        QWidget#deckTitleBar {
            background: transparent;
        }
        QLabel#deckKicker {
            color: #748293;
            font-size: 12px;
            font-weight: 700;
            letter-spacing: 0px;
        }
        QLabel#deckTitle {
            color: #eef3f8;
            font-size: 18px;
            font-weight: 800;
            letter-spacing: 0px;
        }
        QWidget#commandToolbar {
            background: #131a22;
            border: 1px solid #26313d;
            border-radius: 6px;
        }
        QWidget#phoneGridContainer, QWidget#DeviceGridViewport {
            background: #0d1117;
        }
        QLabel {
            background: transparent;
            color: #eef3f8;
        }
        QScrollArea {
            background: #0d1117;
            border: 0;
        }
        QScrollArea#DeviceCanvasScroll {
            background: #0d1117;
            border: 1px solid #26313d;
            border-radius: 6px;
        }
        QGroupBox {
            border: 1px solid #26313d;
            border-radius: 6px;
            margin-top: 16px;
            padding: 16px 12px 12px 12px;
            background: #131a22;
            font-weight: 700;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 5px;
            color: #eef3f8;
            background: #131a22;
        }
        QPushButton, QToolButton {
            border: 1px solid #354454;
            border-radius: 6px;
            padding: 8px 12px;
            min-height: 20px;
            background: #18212b;
            color: #eef3f8;
            font-weight: 700;
        }
        QPushButton:hover, QToolButton:hover {
            background: #202b38;
            border-color: #13a89e;
            color: #ffffff;
        }
        QPushButton:pressed, QToolButton:pressed {
            background: #0f151c;
        }
        QPushButton#primaryButton {
            background: #13a89e;
            border-color: #13a89e;
            color: #ffffff;
        }
        QPushButton#dangerButton {
            background: #2a151b;
            border-color: #d75f6a;
            color: #ffd0d4;
        }
        QPushButton#toggleButton[active="true"] {
            background: #123030;
            border-color: #13a89e;
            color: #eef3f8;
        }
        QFrame#FleetStatusBar QPushButton {
            min-width: 76px;
            min-height: 34px;
            padding: 6px 10px;
            border-radius: 9px;
        }
        QPushButton#dockPrimary {
            background: #13a89e;
            color: #ffffff;
            border-color: #13a89e;
        }
        QPushButton#dockButton {
            background: #18212b;
            color: #eef3f8;
            border-color: #354454;
        }
        QLabel#fleetStatus {
            color: #a7b2c0;
            padding: 0 8px;
            font-weight: 700;
        }
        QLineEdit, QTextEdit, QPlainTextEdit, QComboBox {
            border: 1px solid #26313d;
            border-radius: 6px;
            padding: 7px 9px;
            background: #0f151c;
            selection-background-color: #1f625f;
            selection-color: #ffffff;
            color: #eef3f8;
        }
        QListWidget, QPlainTextEdit {
            border: 1px solid #26313d;
            border-radius: 6px;
            background: #0f151c;
            padding: 6px;
        }
        QListWidget::item {
            padding: 7px 8px;
            border-radius: 5px;
            color: #a7b2c0;
        }
        QListWidget::item:selected {
            background: #123030;
            color: #eef3f8;
        }
        QCheckBox {
            spacing: 7px;
            color: #a7b2c0;
            background: transparent;
        }
        QCheckBox::indicator {
            width: 15px;
            height: 15px;
            border: 1px solid #354454;
            border-radius: 4px;
            background: #0f151c;
        }
        QCheckBox::indicator:checked {
            background: #13a89e;
            border-color: #13a89e;
        }
        QScrollBar:vertical {
            width: 10px;
            background: #0f151c;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background: #354454;
            border-radius: 5px;
            min-height: 40px;
        }
        QScrollBar:horizontal {
            height: 10px;
            background: #0f151c;
            border-radius: 5px;
        }
        QScrollBar::handle:horizontal {
            background: #354454;
            border-radius: 5px;
            min-width: 40px;
        }
        QWidget#phoneGridCard {
            background: #18212b;
            border: 1px solid #26313d;
            border-radius: 6px;
        }
        QWidget#phoneGridCard[selected="true"] {
            background: #17282b;
            border: 2px solid #13a89e;
        }
        QWidget#phoneGridCard[state="Failed"],
        QWidget#phoneGridCard[state="error"] {
            border-color: #d75f6a;
        }
        QWidget#phoneCardHeader {
            background: #131a22;
            border: 1px solid #26313d;
            border-radius: 5px;
        }
        QWidget#phoneGridCard[selected="true"] QWidget#phoneCardHeader {
            border-color: #13a89e;
        }
        QLabel#mutedHint {
            color: #748293;
            font-size: 12px;
            background: transparent;
        }
        QMenu {
            background: #131a22;
            border: 1px solid #354454;
            border-radius: 6px;
            padding: 6px;
        }
        QMenu::item {
            padding: 7px 28px 7px 10px;
            border-radius: 5px;
            color: #eef3f8;
        }
        QMenu::item:selected {
            background: #123030;
            color: #ffffff;
        }
    )");
}

void Dialog::updateBootConfig(bool toView)
{
    if (toView) {
        UserBootConfig config = Config::getInstance().getUserBootConfig();

        if (config.bitRate == 0) {
            ui->bitRateBox->setCurrentText("Mbps");
        } else if (config.bitRate % 1000000 == 0) {
            ui->bitRateEdit->setText(QString::number(config.bitRate / 1000000));
            ui->bitRateBox->setCurrentText("Mbps");
        } else {
            ui->bitRateEdit->setText(QString::number(config.bitRate / 1000));
            ui->bitRateBox->setCurrentText("Kbps");
        }

        ui->maxSizeBox->setCurrentIndex(config.maxSizeIndex);
        ui->formatBox->setCurrentIndex(config.recordFormatIndex);
        ui->recordPathEdt->setText(config.recordPath);
        ui->lockOrientationBox->setCurrentIndex(config.lockOrientationIndex);
        ui->framelessCheck->setChecked(config.framelessWindow);
        ui->recordScreenCheck->setChecked(config.recordScreen);
        ui->notDisplayCheck->setChecked(config.recordBackground);
        ui->useReverseCheck->setChecked(config.reverseConnect);
        ui->fpsCheck->setChecked(config.showFPS);
        ui->alwaysTopCheck->setChecked(config.windowOnTop);
        ui->closeScreenCheck->setChecked(config.autoOffScreen);
        ui->stayAwakeCheck->setChecked(config.keepAlive);
        ui->useSingleModeCheck->setChecked(config.simpleMode);
        ui->autoUpdatecheckBox->setChecked(config.autoUpdateDevice);
        ui->showToolbar->setChecked(config.showToolbar);
    } else {
        UserBootConfig config;

        config.bitRate = getBitRate();
        config.maxSizeIndex = ui->maxSizeBox->currentIndex();
        config.recordFormatIndex = ui->formatBox->currentIndex();
        config.recordPath = ui->recordPathEdt->text();
        config.lockOrientationIndex = ui->lockOrientationBox->currentIndex();
        config.recordScreen = ui->recordScreenCheck->isChecked();
        config.recordBackground = ui->notDisplayCheck->isChecked();
        config.reverseConnect = ui->useReverseCheck->isChecked();
        config.showFPS = ui->fpsCheck->isChecked();
        config.windowOnTop = ui->alwaysTopCheck->isChecked();
        config.autoOffScreen = ui->closeScreenCheck->isChecked();
        config.framelessWindow = ui->framelessCheck->isChecked();
        config.keepAlive = ui->stayAwakeCheck->isChecked();
        config.simpleMode = ui->useSingleModeCheck->isChecked();
        config.autoUpdateDevice = ui->autoUpdatecheckBox->isChecked();
        config.showToolbar = ui->showToolbar->isChecked();

        // 保存当前IP到历史记录
        QString currentIp = ui->deviceIpEdt->currentText().trimmed();
        if (!currentIp.isEmpty()) {
            saveIpHistory(currentIp);
        }

        Config::getInstance().setUserBootConfig(config);
    }
}

void Dialog::execAdbCmd()
{
    if (checkAdbRun()) {
        return;
    }
    QString cmd = ui->adbCommandEdt->text().trimmed();
    outLog("adb " + cmd, false);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    m_adb.execute(ui->serialBox->currentText().trimmed(), cmd.split(" ", Qt::SkipEmptyParts));
#else
    m_adb.execute(ui->serialBox->currentText().trimmed(), cmd.split(" ", QString::SkipEmptyParts));
#endif
}

void Dialog::delayMs(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void Dialog::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
#ifdef Q_OS_WIN32
        this->show();
#endif
        break;
    default:
        break;
    }
}

void Dialog::closeEvent(QCloseEvent *event)
{
    this->hide();
    if (!Config::getInstance().getTrayMessageShown()) {
        Config::getInstance().setTrayMessageShown(true);
        m_hideIcon->showMessage(tr("Notice"),
                                tr("Hidden here!"),
                                QSystemTrayIcon::Information,
                                3000);
    }
    event->ignore();
}

void Dialog::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_expandedOverlay) {
        m_expandedOverlay->setGeometry(rect().adjusted(28, 76, -28, -28));
    }
    relayoutMPhoneScreens();
}

void Dialog::on_updateDevice_clicked()
{
    if (checkAdbRun()) {
        return;
    }
    outLog("update devices...", false);
    m_adb.execute("", QStringList() << "devices");
}

void Dialog::on_startServerBtn_clicked()
{
    startDeviceServer(ui->serialBox->currentText().trimmed());
}

void Dialog::startDeviceServer(const QString &serial)
{
    if (serial.isEmpty()) {
        return;
    }
    if (qsc::IDeviceManage::getInstance().getDevice(serial)) {
        outLog("already connected: " + serial, false);
        return;
    }

    setMPhoneCard(serial, makeStatusCard(serial, "Starting", "Waiting for video handshake"));
    outLog("start server: " + serial, false);

    // this is ok that "original" toUshort is 0
    quint16 videoSize = ui->maxSizeBox->currentText().trimmed().toUShort();
    if (ui->serialBox->count() > 4 && videoSize > 720) {
        videoSize = 720;
    }
    qsc::DeviceParams params;
    params.serial = serial;
    params.maxSize = videoSize;
    params.bitRate = getBitRate();
    if (ui->serialBox->count() > 4 && params.bitRate > 1000000) {
        params.bitRate = 1000000;
    }
    // on devices with Android >= 10, the capture frame rate can be limited
    params.maxFps = static_cast<quint32>(Config::getInstance().getMaxFps());
    params.closeScreen = ui->closeScreenCheck->isChecked();
    params.useReverse = ui->useReverseCheck->isChecked();
    params.display = !ui->notDisplayCheck->isChecked();
    params.control = hasAndroidService(serial, "input");
    if (!params.control) {
        outLog("input service missing, starting display-only stream: " + serial, false);
    }
    params.renderExpiredFrames = Config::getInstance().getRenderExpiredFrames();
    if (ui->lockOrientationBox->currentIndex() > 0) {
        params.captureOrientationLock = 1;
        params.captureOrientation = (ui->lockOrientationBox->currentIndex() - 1) * 90;
    }
    params.stayAwake = ui->stayAwakeCheck->isChecked();
    params.recordFile = ui->recordScreenCheck->isChecked();
    params.recordPath = ui->recordPathEdt->text().trimmed();
    params.recordFileFormat = ui->formatBox->currentText().trimmed();
    params.serverLocalPath = getServerPath();
    params.serverRemotePath = Config::getInstance().getServerPath();
    params.pushFilePath = Config::getInstance().getPushFilePath();
    params.gameScript.clear();
    params.logLevel = Config::getInstance().getLogLevel();
    params.codecOptions = Config::getInstance().getCodecOptions();
    params.codecName = Config::getInstance().getCodecName();
    params.scid = QRandomGenerator::global()->bounded(1, 10000) & 0x7FFFFFFF;

    qsc::IDeviceManage::getInstance().connectDevice(params);
}

void Dialog::startAllDeviceScreens()
{
    m_startQueue.clear();
    m_startAttempts.clear();
    m_recoverAttempts.clear();
    m_activeStarts = 0;

    for (int i = 0; i < ui->serialBox->count(); ++i) {
        const QString serial = ui->serialBox->itemText(i).trimmed();
        if (!serial.isEmpty()) {
            m_startQueue.append(serial);
            setMPhoneCard(serial, makeStatusCard(serial, "Queued", "Waiting for start slot"));
        }
    }
    pumpDeviceStartQueue();
}

void Dialog::pumpDeviceStartQueue()
{
    while (m_activeStarts < m_maxConcurrentStarts && !m_startQueue.isEmpty()) {
        const QString serial = m_startQueue.takeFirst();
        if (serial.isEmpty() || qsc::IDeviceManage::getInstance().getDevice(serial)) {
            continue;
        }

        m_startAttempts[serial] = m_startAttempts.value(serial, 0) + 1;
        ++m_activeStarts;
        startDeviceServer(serial);
    }
}

void Dialog::toggleLeftPanel()
{
    if (!m_devicePoolPanel) {
        return;
    }
    const bool showPanel = !m_devicePoolPanel->isVisible();
    m_devicePoolPanel->setVisible(showPanel);
    if (QWidget *shell = m_devicePoolPanel->parentWidget()) {
        shell->setMinimumWidth(showPanel ? 374 : 144);
        shell->setMaximumWidth(showPanel ? 480 : 144);
    }
    m_toggleLeftButton->setText(showPanel ? i18n("Hide devices") : i18n("Devices"));
    if (showPanel && m_rightConfigScroll && m_rightConfigScroll->isVisible()) {
        m_rightConfigScroll->setVisible(false);
        m_toggleRightButton->setText(i18n("Inspector"));
    }
    relayoutMPhoneScreens();
}

void Dialog::toggleRightPanel()
{
    const bool showPanel = !m_rightConfigScroll->isVisible();
    m_rightConfigScroll->setVisible(showPanel);
    m_toggleRightButton->setText(showPanel ? i18n("Hide inspector") : i18n("Inspector"));
    if (showPanel && m_devicePoolPanel && m_devicePoolPanel->isVisible()) {
        m_devicePoolPanel->setVisible(false);
        if (QWidget *shell = m_devicePoolPanel->parentWidget()) {
            shell->setMinimumWidth(144);
            shell->setMaximumWidth(144);
        }
        m_toggleLeftButton->setText(i18n("Devices"));
    }
    relayoutMPhoneScreens();
}

void Dialog::on_stopServerBtn_clicked()
{
    if (qsc::IDeviceManage::getInstance().disconnectDevice(ui->serialBox->currentText().trimmed())) {
        outLog("stop server");
    }
}

void Dialog::on_wirelessConnectBtn_clicked()
{
    if (checkAdbRun()) {
        return;
    }
    QString addr = ui->deviceIpEdt->currentText().trimmed();
    if (addr.isEmpty()) {
        outLog("error: device ip is null", false);
        return;
    }

    if (!ui->devicePortEdt->currentText().isEmpty()) {
        addr += ":";
        addr += ui->devicePortEdt->currentText().trimmed();
    } else if (!ui->devicePortEdt->lineEdit()->placeholderText().isEmpty()) {
        addr += ":";
        addr += ui->devicePortEdt->lineEdit()->placeholderText().trimmed();
    } else {
        outLog("error: device port is null", false);
        return;
    }

    // 保存IP历史记录 - 只保存IP部分,不包含端口
    QString ip = addr.split(":").first();
    if (!ip.isEmpty()) {
        saveIpHistory(ip);
    }
    
    // 保存端口历史记录
    QString port = addr.split(":").last();
    if (!port.isEmpty() && port != ip) {
        savePortHistory(port);
    }

    outLog("wireless connect...", false);
    QStringList adbArgs;
    adbArgs << "connect";
    adbArgs << addr;
    m_adb.execute("", adbArgs);
}

void Dialog::on_startAdbdBtn_clicked()
{
    if (checkAdbRun()) {
        return;
    }
    outLog("start devices adbd...", false);
    // adb tcpip 5555
    QStringList adbArgs;
    adbArgs << "tcpip";
    adbArgs << "5555";
    m_adb.execute(ui->serialBox->currentText().trimmed(), adbArgs);
}

void Dialog::outLog(const QString &log, bool newLine)
{
    // avoid sub thread update ui
    QString backLog = log;
    QTimer::singleShot(0, this, [this, backLog, newLine]() {
        ui->outEdit->append(backLog);
        if (newLine) {
            ui->outEdit->append("<br/>");
        }
    });
}

bool Dialog::filterLog(const QString &log)
{
    if (log.contains("app_proces")) {
        return true;
    }
    if (log.contains("Unable to set geometry")) {
        return true;
    }
    return false;
}

bool Dialog::checkAdbRun()
{
    if (m_adb.isRuning()) {
        outLog("wait for the end of the current command to run");
    }
    return m_adb.isRuning();
}

void Dialog::on_getIPBtn_clicked()
{
    if (checkAdbRun()) {
        return;
    }

    outLog("get ip...", false);
    // adb -s P7C0218510000537 shell ifconfig wlan0
    // or
    // adb -s P7C0218510000537 shell ip -f inet addr show wlan0
    QStringList adbArgs;
#if 0
    adbArgs << "shell";
    adbArgs << "ip";
    adbArgs << "-f";
    adbArgs << "inet";
    adbArgs << "addr";
    adbArgs << "show";
    adbArgs << "wlan0";
#else
    adbArgs << "shell";
    adbArgs << "ifconfig";
    adbArgs << "wlan0";
#endif
    m_adb.execute(ui->serialBox->currentText().trimmed(), adbArgs);
}

void Dialog::getIPbyIp()
{
    if (checkAdbRun()) {
        return;
    }

    QStringList adbArgs;
    adbArgs << "shell";
    adbArgs << "ip -o a";

    m_adb.execute(ui->serialBox->currentText().trimmed(), adbArgs);
}

void Dialog::onDeviceConnected(bool success, const QString &serial, const QString &deviceName, const QSize &size)
{
    if (m_activeStarts > 0) {
        --m_activeStarts;
    }

    if (!success) {
        outLog("connect failed: " + serial, true);
        static const int maxAttempts = 5;
        static const int maxRecoveries = 3;
        const int attempts = m_startAttempts.value(serial, 1);
        if (attempts < maxAttempts) {
            setMPhoneCard(serial, makeStatusCard(serial, "Retrying", QString("Handshake failed, retry %1 of %2").arg(attempts).arg(maxAttempts - 1)));
            if (!m_startQueue.contains(serial)) {
                m_startQueue.append(serial);
            }
            QTimer::singleShot(350, this, &Dialog::pumpDeviceStartQueue);
        } else {
            const int recoveries = m_recoverAttempts.value(serial, 0);
            if (recoveries < maxRecoveries) {
                m_recoverAttempts[serial] = recoveries + 1;
                setMPhoneCard(serial, makeStatusCard(serial, "Recovering", QString("Resetting stream path, recovery %1 of %2").arg(recoveries + 1).arg(maxRecoveries)));
                QTimer::singleShot(1800 + recoveries * 700, this, [this, serial]() {
                    if (!ui || ui->serialBox->findText(serial) < 0 || qsc::IDeviceManage::getInstance().getDevice(serial)) {
                        pumpDeviceStartQueue();
                        return;
                    }
                    QWidget *oldCard = m_mphoneCards.take(serial);
                    if (oldCard) {
                        oldCard->deleteLater();
                    }
                    m_startAttempts[serial] = 0;
                    setMPhoneCard(serial, makeStatusCard(serial, "Queued", "Recovering failed stream"));
                    if (!m_startQueue.contains(serial)) {
                        m_startQueue.append(serial);
                    }
                    pumpDeviceStartQueue();
                });
            } else {
                setMPhoneCard(serial, makeStatusCard(serial, "Failed", "Video/control handshake did not complete"));
            }
            QTimer::singleShot(200, this, &Dialog::pumpDeviceStartQueue);
        }
        return;
    }
    outLog(QString("screen connected: %1 (%2)").arg(deviceName, serial), true);
    m_startAttempts.remove(serial);
    m_recoverAttempts.remove(serial);
    auto videoForm = new VideoForm(false, false, false, m_mphoneContainer);
    videoForm->setEmbeddedMode(true);
    videoForm->setSerial(serial);
    connect(videoForm, &VideoForm::mirrorMouseInput, this, &Dialog::mirrorMouseInput);
    connect(videoForm, &VideoForm::mirrorWheelInput, this, &Dialog::mirrorWheelInput);
    connect(videoForm, &VideoForm::mirrorKeyInput, this, &Dialog::mirrorKeyInput);

    qsc::IDeviceManage::getInstance().getDevice(serial)->setUserData(static_cast<void*>(videoForm));
    qsc::IDeviceManage::getInstance().getDevice(serial)->registerDeviceObserver(videoForm);


    videoForm->showFPS(ui->fpsCheck->isChecked());
    QString name = Config::getInstance().getNickName(serial);
    if (name.isEmpty()) {
        name = Config::getInstance().getTitle();
    }
    videoForm->setWindowTitle(name + "-" + serial);
    videoForm->updateShowSize(size);
    setMPhoneCard(serial, makeVideoCard(serial, videoForm));

    GroupController::instance().addDevice(serial);
    updateFleetMetrics();
    QTimer::singleShot(80, this, &Dialog::pumpDeviceStartQueue);
}

void Dialog::onDeviceDisconnected(QString serial)
{
    GroupController::instance().removeDevice(serial);
    m_selectedDevices.remove(serial);
    m_startAttempts.remove(serial);
    m_recoverAttempts.remove(serial);
    updateFleetSelectionUi();
    auto device = qsc::IDeviceManage::getInstance().getDevice(serial);
    if (!device) {
        return;
    }
    auto data = device->getUserData();
    if (data) {
        VideoForm* vf = static_cast<VideoForm*>(data);
        qsc::IDeviceManage::getInstance().getDevice(serial)->deRegisterDeviceObserver(vf);
        QWidget *card = m_mphoneCards.take(serial);
        relayoutMPhoneScreens();
        if (card) {
            card->deleteLater();
        } else {
            vf->close();
            vf->deleteLater();
        }
    }
    updateFleetMetrics();
}

void Dialog::relayoutMPhoneScreens()
{
    if (!m_mphoneGrid) {
        return;
    }

    clearMPhoneGrid();

    const int count = m_mphoneCards.size();
    if (count == 0) {
        auto *emptyLabel = new QLabel(m_mphoneContainer);
        emptyLabel->setObjectName("emptyDeviceState");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setText(QString("No streams running\n\n%1 devices discovered. Start selected devices or double click a device.").arg(ui->serialBox->count()));
        emptyLabel->setStyleSheet(R"(
            QLabel#emptyDeviceState {
                color: #a7b2c0;
                font-size: 18px;
                font-weight: 700;
                border: 1px solid #26313d;
                border-radius: 6px;
                background: #131a22;
                padding: 40px;
            }
        )");
        emptyLabel->setMinimumSize(260, 220);
        emptyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mphoneGrid->addWidget(emptyLabel, 0, 0);
        updateFleetSelectionUi();
        return;
    }

    const int tileWidth = 240;
    const int viewportWidth = m_mphoneScroll ? qMax(1, m_mphoneScroll->viewport()->width()) : width();
    const int columns = qMax(1, viewportWidth / (tileWidth + m_mphoneGrid->spacing()));
    for (int column = 0; column < columns; ++column) {
        m_mphoneGrid->setColumnMinimumWidth(column, tileWidth);
        m_mphoneGrid->setColumnStretch(column, 1);
    }
    int index = 0;
    for (QWidget *screen : m_mphoneCards) {
        m_mphoneGrid->addWidget(screen, index / columns, index % columns);
        ++index;
    }
    m_mphoneGrid->setRowStretch((count + columns - 1) / columns, 1);
    updateFleetSelectionUi();
}

void Dialog::clearMPhoneGrid()
{
    if (!m_mphoneGrid) {
        return;
    }

    while (QLayoutItem *item = m_mphoneGrid->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            if (m_mphoneCards.values().contains(widget)) {
                widget->setParent(m_mphoneContainer);
            } else {
                widget->deleteLater();
            }
        }
        delete item;
    }
}

void Dialog::setMPhoneCard(const QString &serial, QWidget *card)
{
    if (serial.isEmpty() || !card) {
        return;
    }

    if (m_mphoneCards.contains(serial)) {
        QWidget *oldCard = m_mphoneCards.take(serial);
        if (oldCard && oldCard != card) {
            oldCard->deleteLater();
        }
    }

    card->setParent(m_mphoneContainer);
    m_mphoneCards.insert(serial, card);
    relayoutMPhoneScreens();
    card->show();
}

QWidget *Dialog::makeStatusCard(const QString &serial, const QString &status, const QString &details)
{
    auto *card = new QWidget(m_mphoneContainer);
    card->setObjectName("phoneGridCard");
    card->setProperty("serial", serial);
    card->setProperty("state", status);
    card->setMinimumSize(220, 330);
    card->setMaximumHeight(380);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(8);

    auto *titleRow = new QWidget(card);
    auto *titleLayout = new QHBoxLayout(titleRow);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(8);
    auto *selectButton = makeDeviceActionButton("Select", "Select this device", titleRow);
    selectButton->setCheckable(true);
    selectButton->setFixedWidth(34);
    setMPhoneIcon(selectButton, QChar(0xf10c));
    auto *titleLabel = new QLabel(serial, titleRow);
    titleLabel->setWordWrap(true);
    titleLabel->setObjectName("DeviceCardTitle");
    titleLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #eef3f8;");
    titleLayout->addWidget(selectButton, 0);
    titleLayout->addWidget(titleLabel, 1);
    connect(selectButton, &QPushButton::clicked, this, [this, serial](bool checked) {
        setDeviceSelected(serial, checked);
    });
    card->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(card, &QWidget::customContextMenuRequested, this, [this, serial, card](const QPoint &pos) {
        showDeviceContextMenu(serial, nullptr, card->mapToGlobal(pos));
    });

    auto *statusLabel = new QLabel(i18n(status), card);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 18px; font-weight: 800; color: #eef3f8; background: #0f151c; border: 1px solid #354454; border-radius: 6px; padding: 10px;");

    auto *detailsLabel = new QLabel(i18n(details), card);
    detailsLabel->setWordWrap(true);
    detailsLabel->setAlignment(Qt::AlignCenter);
    detailsLabel->setStyleSheet("color: #a7b2c0; background: #131a22; border-radius: 6px; padding: 8px;");

    layout->addWidget(titleRow);
    layout->addStretch(1);
    layout->addWidget(statusLabel);
    layout->addWidget(detailsLabel);
    layout->addStretch(1);
    return card;
}

QWidget *Dialog::makeVideoCard(const QString &serial, VideoForm *videoForm)
{
    auto *card = new QWidget(m_mphoneContainer);
    card->setObjectName("phoneGridCard");
    card->setProperty("serial", serial);
    card->setProperty("state", "Streaming");
    card->setMinimumSize(220, 330);
    card->setMaximumHeight(380);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    auto *header = new QWidget(card);
    header->setObjectName("phoneCardHeader");
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(8, 6, 6, 6);
    headerLayout->setSpacing(6);

    auto *selectButton = makeDeviceActionButton("Select", "Select this device", header);
    selectButton->setCheckable(true);
    selectButton->setFixedWidth(32);
    setMPhoneIcon(selectButton, QChar(0xf10c));

    auto *titleLabel = new QLabel(serial, header);
    titleLabel->setWordWrap(true);
    titleLabel->setToolTip(serial);
    titleLabel->setTextFormat(Qt::PlainText);
    titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    titleLabel->setMaximumHeight(28);
    titleLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    titleLabel->setStyleSheet("font-size: 13px; font-weight: 700; background: transparent; color: #eef3f8;");

    auto *expandButton = makeDeviceActionButton("Expand", "Expand this screen", header);
    auto *restartButton = makeDeviceActionButton("Restart", "Restart this screen", header);
    auto *closeButton = makeDeviceActionButton("Stop", "Stop this screen", header);
    expandButton->setFixedWidth(32);
    restartButton->setFixedWidth(32);
    closeButton->setFixedWidth(32);
    connect(expandButton, &QPushButton::clicked, this, [this, serial, videoForm]() {
        showExpandedDevice(serial, videoForm);
    });
    connect(restartButton, &QPushButton::clicked, this, [this, serial]() {
        qsc::IDeviceManage::getInstance().disconnectDevice(serial);
        QTimer::singleShot(800, this, [this, serial]() { startDeviceServer(serial); });
    });
    connect(closeButton, &QPushButton::clicked, this, [serial]() {
        qsc::IDeviceManage::getInstance().disconnectDevice(serial);
    });

    connect(selectButton, &QPushButton::clicked, this, [this, serial](bool checked) {
        setDeviceSelected(serial, checked);
    });
    card->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(card, &QWidget::customContextMenuRequested, this, [this, serial, videoForm, card](const QPoint &pos) {
        showDeviceContextMenu(serial, videoForm, card->mapToGlobal(pos));
    });

    headerLayout->addWidget(selectButton);
    headerLayout->addWidget(titleLabel, 1);
    headerLayout->addWidget(expandButton);
    headerLayout->addWidget(restartButton);
    headerLayout->addWidget(closeButton);

    videoForm->setParent(card);
    videoForm->setMinimumSize(0, 0);
    videoForm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addWidget(header, 0);
    layout->addWidget(videoForm, 1);

    auto *actionBar = new QWidget(card);
    auto *actionLayout = new QHBoxLayout(actionBar);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(6);

    const QList<QPair<QString, QString>> primaryActions = {
        {"Back", "back"},
        {"Home", "home"},
        {"Recent", "recent"},
    };

    for (const auto &item : primaryActions) {
        QPushButton *button = makeDeviceActionButton(item.first, item.second, actionBar);
        connect(button, &QPushButton::clicked, this, [this, serial, action = item.second]() {
            runDeviceActionOnTargets(serial, action);
        });
        actionLayout->addWidget(button, 1);
    }

    auto *moreButton = makeDeviceActionButton("More", "More actions", actionBar);
    connect(moreButton, &QPushButton::clicked, this, [this, serial, videoForm, moreButton]() {
        showDeviceContextMenu(serial, videoForm, moreButton->mapToGlobal(QPoint(0, moreButton->height())));
    });
    actionLayout->addWidget(moreButton, 0);

    layout->addWidget(actionBar, 0);
    return card;
}

QPushButton *Dialog::makeDeviceActionButton(const QString &label, const QString &tooltip, QWidget *parent)
{
    auto *button = new QPushButton(label, parent);
    button->setToolTip(tooltip);
    if (label == "Back") {
        setMPhoneIcon(button, QChar(0xf053));
    } else if (label == "Home") {
        setMPhoneIcon(button, QChar(0xf015));
    } else if (label == "Recent") {
        setMPhoneIcon(button, QChar(0xf24d));
    } else if (label == "More") {
        setMPhoneIcon(button, QChar(0xf141));
    } else if (label == "Expand") {
        setMPhoneIcon(button, QChar(0xf065));
    } else if (label == "Restart") {
        setMPhoneIcon(button, QChar(0xf021));
    } else if (label == "Stop" || label == "Close") {
        setMPhoneIcon(button, QChar(0xf00d), QColor("#d92d4b"));
    } else if (label == "Power") {
        setMPhoneIcon(button, QChar(0xf011));
    } else if (label == "Vol-") {
        setMPhoneIcon(button, QChar(0xf027));
    } else if (label == "Vol+") {
        setMPhoneIcon(button, QChar(0xf028));
    } else if (label == "Wake") {
        setMPhoneIcon(button, QChar(0xf06e));
    } else if (label == "Sleep") {
        setMPhoneIcon(button, QChar(0xf070));
    } else if (label == "Shot") {
        setMPhoneIcon(button, QChar(0xf030));
    } else if (label == "Group") {
        setMPhoneIcon(button, QChar(0xf0c0));
    } else if (label == "Select") {
        setMPhoneIcon(button, QChar(0xf10c));
    }
    if (label == "Back" || label == "Home" || label == "Recent" || label == "More" ||
        label == "Expand" || label == "Restart" || label == "Stop" || label == "Close" || label == "Select") {
        button->setText("");
    }
    button->setMinimumWidth(0);
    button->setFixedHeight(32);
    button->setMaximumHeight(32);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    button->setStyleSheet("QPushButton { padding: 4px 8px; font-size: 12px; border-radius: 6px; background: #131a22; border: 1px solid #354454; color: #eef3f8; font-weight: 700; } QPushButton:hover { background: #202b38; border-color: #13a89e; color: #ffffff; } QPushButton:pressed { background: #0f151c; } QPushButton:checked { background: #123030; border-color: #13a89e; color: #eef3f8; }");
    return button;
}

void Dialog::setDeviceSelected(const QString &serial, bool selected)
{
    if (serial.isEmpty()) {
        return;
    }
    if (selected) {
        m_selectedDevices.insert(serial);
    } else {
        m_selectedDevices.remove(serial);
    }

    QWidget *card = m_mphoneCards.value(serial, nullptr);
    if (card) {
        card->setProperty("selected", selected);
        card->style()->unpolish(card);
        card->style()->polish(card);
        card->update();
        const QList<QPushButton *> buttons = card->findChildren<QPushButton *>();
        for (QPushButton *button : buttons) {
            if (button->toolTip() == "Select this device") {
                button->blockSignals(true);
                button->setChecked(selected);
                setMPhoneIcon(button, selected ? QChar(0xf058) : QChar(0xf10c),
                              selected ? QColor("#13a89e") : QColor("#a7b2c0"));
                button->blockSignals(false);
            }
        }
    }
    updateFleetSelectionUi();
}

void Dialog::selectAllDeviceCards()
{
    for (const QString &serial : m_mphoneCards.keys()) {
        setDeviceSelected(serial, true);
    }
    updateFleetSelectionUi();
}

void Dialog::clearDeviceSelection()
{
    const QStringList serials = m_selectedDevices.values();
    for (const QString &serial : serials) {
        setDeviceSelected(serial, false);
    }
    updateFleetSelectionUi();
}

void Dialog::invertDeviceSelection()
{
    for (const QString &serial : m_mphoneCards.keys()) {
        setDeviceSelected(serial, !m_selectedDevices.contains(serial));
    }
    updateFleetSelectionUi();
}

void Dialog::applyDevicePoolFilter()
{
    if (!ui || !ui->connectedPhoneList) {
        return;
    }

    const QString query = m_deviceSearchEdit ? m_deviceSearchEdit->text().trimmed().toLower() : QString();
    const QString filter = m_deviceFilterBox ? m_deviceFilterBox->currentData().toString() : QString("all");
    for (int i = 0; i < ui->connectedPhoneList->count(); ++i) {
        QListWidgetItem *item = ui->connectedPhoneList->item(i);
        if (!item) {
            continue;
        }

        const QString serial = i < ui->serialBox->count() ? ui->serialBox->itemText(i).trimmed() : QString();
        const QWidget *card = m_mphoneCards.value(serial, nullptr);
        const QString state = card ? card->property("state").toString() : QString("Online");
        bool visible = query.isEmpty() || item->text().toLower().contains(query) || serial.toLower().contains(query);
        if (visible && filter == "streaming") {
            visible = state == "Streaming";
        } else if (visible && filter == "selected") {
            visible = m_selectedDevices.contains(serial);
        } else if (visible && filter == "pending") {
            visible = state == "Queued" || state == "Starting" || state == "Retrying" || state == "Recovering";
        } else if (visible && filter == "failed") {
            visible = state == "Failed";
        } else if (visible && filter == "online") {
            visible = !serial.isEmpty();
        }
        item->setHidden(!visible);
    }
}

QStringList Dialog::visibleDeviceSerials() const
{
    QStringList serials;
    if (!ui || !ui->connectedPhoneList || !ui->serialBox) {
        return serials;
    }

    for (int i = 0; i < ui->connectedPhoneList->count() && i < ui->serialBox->count(); ++i) {
        const QListWidgetItem *item = ui->connectedPhoneList->item(i);
        const QString serial = ui->serialBox->itemText(i).trimmed();
        if (item && !item->isHidden() && !serial.isEmpty() && !serials.contains(serial)) {
            serials.append(serial);
        }
    }
    return serials;
}

QStringList Dialog::selectedDeviceSerials() const
{
    QStringList serials;
    for (const QString &serial : m_selectedDevices) {
        if (m_mphoneCards.contains(serial) || qsc::IDeviceManage::getInstance().getDevice(serial)) {
            serials.append(serial);
        }
    }
    serials.sort();
    return serials;
}

void Dialog::updateFleetSelectionUi()
{
    if (m_fleetStatusLabel) {
        m_fleetStatusLabel->setText(QString("%1 %2").arg(selectedDeviceSerials().size()).arg(i18n("Selected").toLower()));
    }
    if (m_syncControlButton) {
        m_syncControlButton->setText(m_syncControlEnabled ? i18n("Sync on") : i18n("Sync off"));
        m_syncControlButton->setProperty("active", m_syncControlEnabled);
        m_syncControlButton->style()->unpolish(m_syncControlButton);
        m_syncControlButton->style()->polish(m_syncControlButton);
    }
    if (m_runScriptButton) {
        m_runScriptButton->setToolTip("Run script on chosen target scope");
    }
    updateFleetMetrics();
}

void Dialog::updateFleetMetrics()
{
    const int total = ui && ui->serialBox ? ui->serialBox->count() : 0;
    int running = 0;
    int failed = 0;
    int pending = 0;
    for (QWidget *card : m_mphoneCards) {
        const QString state = card ? card->property("state").toString() : QString();
        if (state == "Streaming") {
            ++running;
        } else if (state == "Failed") {
            ++failed;
        } else if (state == "Queued" || state == "Starting" || state == "Retrying" || state == "Recovering") {
            ++pending;
        }
    }
    const int selected = selectedDeviceSerials().size();

    if (m_totalDevicesMetric) {
        m_totalDevicesMetric->setText(QString("%1 %2").arg(i18n("Devices")).arg(total));
    }
    if (m_runningDevicesMetric) {
        m_runningDevicesMetric->setText(QString("%1 %2").arg(i18n("Streaming")).arg(running));
    }
    if (m_failedDevicesMetric) {
        m_failedDevicesMetric->setText(QString("%1 %2").arg(i18n("Failed")).arg(failed));
        m_failedDevicesMetric->setProperty("active", failed > 0);
        m_failedDevicesMetric->style()->unpolish(m_failedDevicesMetric);
        m_failedDevicesMetric->style()->polish(m_failedDevicesMetric);
    }
    if (m_selectedDevicesMetric) {
        m_selectedDevicesMetric->setText(QString("%1 %2").arg(i18n("Selected")).arg(selected));
    }
    if (m_poolSummaryLabel) {
        m_poolSummaryLabel->setText(QString("%1 %2 · %3 %4 · %5 %6 · %7 %8 · %9 %10")
                                        .arg(total)
                                        .arg(i18n("Devices").toLower())
                                        .arg(running)
                                        .arg(i18n("Streaming").toLower())
                                        .arg(pending)
                                        .arg(i18n("Pending").toLower())
                                        .arg(failed)
                                        .arg(i18n("Failed").toLower())
                                        .arg(selected)
                                        .arg(i18n("Selected").toLower()));
    }
    if (m_queueStatusLabel) {
        const QString text = (m_startQueue.isEmpty() && m_activeStarts == 0)
                                 ? i18n("Queue idle")
                                 : QString("Queue %1 · active %2").arg(m_startQueue.size()).arg(m_activeStarts);
        m_queueStatusLabel->setText(text);
    }
    if (m_adbStatusLabel) {
        m_adbStatusLabel->setText(m_adb.isRuning() ? "ADB running" : i18n("ADB ready"));
    }
    applyDevicePoolFilter();
}

void Dialog::runDeviceActionOnTargets(const QString &sourceSerial, const QString &action)
{
    QStringList targets;
    if (m_syncControlEnabled && m_selectedDevices.contains(sourceSerial)) {
        targets = selectedDeviceSerials();
    }
    if (targets.isEmpty()) {
        targets << sourceSerial;
    }
    for (const QString &serial : targets) {
        runDeviceAction(serial, action);
    }
}

void Dialog::mirrorMouseInput(const QString &sourceSerial, QEvent::Type type, const QPointF &ratioPos,
                              Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
{
    if (!m_syncControlEnabled || !m_selectedDevices.contains(sourceSerial)) {
        return;
    }

    for (const QString &serial : selectedDeviceSerials()) {
        if (serial == sourceSerial) {
            continue;
        }
        auto device = qsc::IDeviceManage::getInstance().getDevice(serial);
        auto *videoForm = qobject_cast<VideoForm *>(m_mphoneCards.value(serial, nullptr)
                                                       ? m_mphoneCards.value(serial)->findChild<VideoForm *>()
                                                       : nullptr);
        if (!device || !videoForm || videoForm->frameSize().isEmpty()) {
            continue;
        }
        const QSize frame = videoForm->frameSize();
        const QSize show(qMax(1, frame.width()), qMax(1, frame.height()));
        QPointF pos(qBound(0.0, ratioPos.x(), 1.0) * show.width(),
                    qBound(0.0, ratioPos.y(), 1.0) * show.height());
        QMouseEvent event(type, pos, pos, button, buttons, modifiers);
        emit device->mouseEvent(&event, frame, show);
    }
}

void Dialog::mirrorWheelInput(const QString &sourceSerial, const QPointF &ratioPos, const QPoint &angleDelta,
                              Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
{
    if (!m_syncControlEnabled || !m_selectedDevices.contains(sourceSerial) || angleDelta.isNull()) {
        return;
    }

    for (const QString &serial : selectedDeviceSerials()) {
        if (serial == sourceSerial) {
            continue;
        }
        auto device = qsc::IDeviceManage::getInstance().getDevice(serial);
        auto *videoForm = qobject_cast<VideoForm *>(m_mphoneCards.value(serial, nullptr)
                                                       ? m_mphoneCards.value(serial)->findChild<VideoForm *>()
                                                       : nullptr);
        if (!device || !videoForm || videoForm->frameSize().isEmpty()) {
            continue;
        }
        const QSize frame = videoForm->frameSize();
        const QSize show(qMax(1, frame.width()), qMax(1, frame.height()));
        QPointF pos(qBound(0.0, ratioPos.x(), 1.0) * show.width(),
                    qBound(0.0, ratioPos.y(), 1.0) * show.height());
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QWheelEvent event(pos, pos, QPoint(), angleDelta, buttons, modifiers, Qt::NoScrollPhase, false);
#else
        QWheelEvent event(pos, pos, QPoint(), angleDelta, 0, Qt::Vertical, buttons, modifiers);
#endif
        emit device->wheelEvent(&event, frame, show);
    }
}

void Dialog::mirrorKeyInput(const QString &sourceSerial, QEvent::Type type, int key, Qt::KeyboardModifiers modifiers,
                            const QString &text, bool autoRepeat, ushort count)
{
    if (!m_syncControlEnabled || !m_selectedDevices.contains(sourceSerial)) {
        return;
    }

    for (const QString &serial : selectedDeviceSerials()) {
        if (serial == sourceSerial) {
            continue;
        }
        auto device = qsc::IDeviceManage::getInstance().getDevice(serial);
        auto *videoForm = qobject_cast<VideoForm *>(m_mphoneCards.value(serial, nullptr)
                                                       ? m_mphoneCards.value(serial)->findChild<VideoForm *>()
                                                       : nullptr);
        if (!device || !videoForm) {
            continue;
        }
        QKeyEvent event(type, key, modifiers, text, autoRepeat, count);
        emit device->keyEvent(&event, videoForm->frameSize(), videoForm->size());
    }
}

void Dialog::showDeviceContextMenu(const QString &serial, VideoForm *videoForm, const QPoint &globalPos)
{
    if (serial.isEmpty()) {
        return;
    }

    QMenu menu(this);
    menu.setObjectName("DeviceCommandMenu");
    menu.setTitle(serial);

    auto addAction = [this, &menu](const QString &text, const QString &command, QChar glyph,
                             const QColor &color = QColor("#d7e4f2")) {
        QAction *action = menu.addAction(mphoneIcon(glyph, color), i18n(text));
        action->setProperty("i18nKey", text);
        action->setData(command);
        return action;
    };
    auto addShellAction = [&addAction](const QString &text, const QString &command, QChar glyph) {
        return addAction(text, command, glyph);
    };

    QAction *title = menu.addAction(mphoneIcon(QChar(0xf10b), QColor("#31e6ad")), serial);
    title->setEnabled(false);
    menu.addSeparator();

    menu.addSection(i18n("Control"));
    addAction("Back", "action:back", QChar(0xf053));
    addAction("Home", "action:home", QChar(0xf015));
    addAction("Recent apps", "action:recent", QChar(0xf24d));
    addAction("Power", "action:power", QChar(0xf011));
    addAction("Wake screen", "action:screen_on", QChar(0xf06e));
    addAction("Sleep screen", "action:screen_off", QChar(0xf070));
    addAction("Volume down", "action:volume_down", QChar(0xf027));
    addAction("Volume up", "action:volume_up", QChar(0xf028));

    menu.addSeparator();
    menu.addSection(i18n("Display"));
    QAction *expandAction = addAction("Focus in workspace", "focus", QChar(0xf065), QColor("#9bd3ff"));
    expandAction->setEnabled(videoForm != nullptr);
    addAction("Screenshot", "action:screenshot", QChar(0xf030));
    addAction("Reconnect stream", "reconnect", QChar(0xf021), QColor("#ffd166"));
    addAction("Stop stream", "stop-stream", QChar(0xf00d), QColor("#ff9a9a"));

    menu.addSeparator();
    menu.addSection(i18n("Batch"));
    addAction("Select device", "select", QChar(0xf058), QColor("#31e6ad"));
    addAction("Make sync master", "sync-master", QChar(0xf0c0), QColor("#9bd3ff"));
    addAction("Run current script here", "script-here", QChar(0xf121), QColor("#f4c95d"));
    addAction("Run current script on selected", "script-selected", QChar(0xf04b), QColor("#31e6ad"));

    menu.addSeparator();
    menu.addSection(i18n("Apps / ADB"));
    addShellAction("Current activity", "shell:activity", QChar(0xf05a));
    addShellAction("List packages", "shell:packages", QChar(0xf187));
    addShellAction("Battery status", "shell:battery", QChar(0xf240));
    addShellAction("Device properties", "shell:props", QChar(0xf085));
    addShellAction("Open app switcher", "shell:app-switch", QChar(0xf12d));
    addAction("Install keepalive agent", "agent-install", QChar(0xf019), QColor("#0b8bdc"));
    addAction("Start keepalive agent", "agent-start", QChar(0xf04b), QColor("#31b47b"));
    addAction("Apply phonebox hardening", "agent-harden", QChar(0xf023), QColor("#31b47b"));
    addAction("Agent status", "agent-status", QChar(0xf0e7), QColor("#f4c95d"));

    menu.addSeparator();
    menu.addSection(i18n("Diagnostics"));
    addAction("Copy serial to log", "log-serial", QChar(0xf0c5));
    addAction("Health check", "health", QChar(0xf0e7), QColor("#31e6ad"));

    QAction *selected = menu.exec(globalPos);
    if (!selected || selected->data().toString().isEmpty()) {
        return;
    }

    const QString command = selected->data().toString();
    outLog(QString("menu command: %1 -> %2").arg(serial, selected->text()), false);

    if (command.startsWith("action:")) {
        runDeviceActionOnTargets(serial, command.mid(QString("action:").size()));
    } else if (command == "focus") {
        showExpandedDevice(serial, videoForm);
    } else if (command == "reconnect") {
        qsc::IDeviceManage::getInstance().disconnectDevice(serial);
        QTimer::singleShot(500, this, [this, serial]() {
            if (!m_startQueue.contains(serial)) {
                m_startQueue.prepend(serial);
            }
            m_startAttempts[serial] = 0;
            pumpDeviceStartQueue();
        });
    } else if (command == "stop-stream") {
        qsc::IDeviceManage::getInstance().disconnectDevice(serial);
    } else if (command == "select") {
        setDeviceSelected(serial, true);
    } else if (command == "sync-master") {
        setDeviceSelected(serial, true);
        m_syncControlEnabled = true;
        updateFleetSelectionUi();
        outLog("sync master ready: " + serial, false);
    } else if (command == "script-here") {
        if (!m_batchScriptEditor) {
            return;
        }
        const QStringList lines = m_batchScriptEditor->toPlainText().split('\n');
        outLog("script run on: " + serial, false);
        for (const QString &rawLine : lines) {
            const QString line = rawLine.trimmed();
            if (line.isEmpty() || line.startsWith("#") || line.startsWith("//")) {
                continue;
            }
            if (line.startsWith("sleep ", Qt::CaseInsensitive)) {
                bool ok = false;
                const int ms = line.section(' ', 1, 1).toInt(&ok);
                delayMs(ok ? ms : 300);
            } else {
                runScriptLineForDevice(serial, line);
            }
        }
    } else if (command == "script-selected") {
        runCurrentBatchScript();
    } else if (command == "shell:activity") {
        runAdbShell(serial, QStringList() << "dumpsys" << "window" << "windows");
    } else if (command == "shell:packages") {
        runAdbShell(serial, QStringList() << "pm" << "list" << "packages");
    } else if (command == "shell:battery") {
        runAdbShell(serial, QStringList() << "cmd" << "battery" << "get" << "level");
        runAdbShell(serial, QStringList() << "cmd" << "battery" << "get" << "status");
        runAdbShell(serial, QStringList() << "cmd" << "battery" << "get" << "temperature");
    } else if (command == "shell:props") {
        runAdbShell(serial, QStringList() << "getprop");
    } else if (command == "shell:app-switch") {
        runAdbShell(serial, QStringList() << "input" << "keyevent" << "APP_SWITCH");
    } else if (command == "agent-install") {
        if (installMPhoneAgent(serial)) {
            startMPhoneAgent(serial);
            applyMPhoneAgentHardening(serial);
        }
    } else if (command == "agent-start") {
        startMPhoneAgent(serial);
    } else if (command == "agent-harden") {
        applyMPhoneAgentHardening(serial);
    } else if (command == "agent-status") {
        showMPhoneAgentStatus(serial);
    } else if (command == "log-serial") {
        outLog("device serial: " + serial, false);
    } else if (command == "health") {
        runAdbShell(serial, QStringList() << "getprop" << "ro.product.model");
        runAdbShell(serial, QStringList() << "wm" << "size");
        runAdbShell(serial, QStringList() << "cmd" << "battery" << "get" << "level");
        outLog("health check queued: " + serial, false);
    }
}

void Dialog::showExpandedDevice(const QString &serial, VideoForm *videoForm)
{
    if (!videoForm || !videoForm->parentWidget() || m_expandedOverlay) {
        return;
    }

    QWidget *card = videoForm->parentWidget();
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());
    if (!cardLayout) {
        return;
    }

    auto *overlay = new QFrame(this);
    m_expandedOverlay = overlay;
    overlay->setObjectName("ExpandedDeviceOverlay");
    overlay->setAttribute(Qt::WA_DeleteOnClose);
    overlay->setGeometry(rect().adjusted(28, 76, -28, -28));
    overlay->setStyleSheet(R"(
        QFrame#ExpandedDeviceOverlay {
            background: #0d1117;
            border: 1px solid #354454;
            border-radius: 6px;
        }
        QFrame#ExpandedDeviceOverlay QLabel {
            color: #eef3f8;
            background: transparent;
            font-weight: 800;
        }
        QFrame#ExpandedDeviceOverlay QPushButton {
            background: #18212b;
            color: #eef3f8;
            border: 1px solid #354454;
            border-radius: 6px;
            padding: 9px 14px;
            font-weight: 700;
        }
        QFrame#ExpandedDeviceOverlay QPushButton:hover {
            background: #202b38;
            border-color: #13a89e;
        }
    )");

    auto *overlayLayout = new QVBoxLayout(overlay);
    overlayLayout->setContentsMargins(18, 18, 18, 18);
    overlayLayout->setSpacing(12);

    auto *header = new QWidget(overlay);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    auto *title = new QLabel(serial, header);
    title->setStyleSheet("font-size: 20px;");
    auto *close = makeDeviceActionButton("Close", "Close expanded view", header);
    close->setMinimumWidth(92);
    headerLayout->addWidget(title, 1);
    headerLayout->addWidget(close);

    videoForm->setParent(overlay);
    videoForm->setMinimumSize(360, 520);
    videoForm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *actions = new QWidget(overlay);
    auto *actionLayout = new QHBoxLayout(actions);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(8);
    const QList<QPair<QString, QString>> primaryActions = {
        {"Back", "back"}, {"Home", "home"}, {"Recent", "recent"},
        {"Power", "power"}, {"Shot", "screenshot"},
    };
    for (const auto &item : primaryActions) {
        QPushButton *button = makeDeviceActionButton(item.first, item.first, actions);
        connect(button, &QPushButton::clicked, this, [this, serial, action = item.second]() {
            runDeviceAction(serial, action);
        });
        actionLayout->addWidget(button, 1);
    }

    overlayLayout->addWidget(header, 0);
    overlayLayout->addWidget(videoForm, 1);
    overlayLayout->addWidget(actions, 0);

    auto restore = [this, cardLayout, videoForm, overlay]() {
        if (!videoForm) {
            return;
        }
        videoForm->setParent(cardLayout->parentWidget());
        videoForm->setMinimumSize(0, 0);
        cardLayout->insertWidget(1, videoForm, 1);
        videoForm->show();
        if (m_expandedOverlay == overlay) {
            m_expandedOverlay.clear();
        }
        overlay->deleteLater();
    };
    connect(close, &QPushButton::clicked, this, restore);
    overlay->raise();
    overlay->show();
    videoForm->show();
}

void Dialog::runDeviceAction(const QString &serial, const QString &action)
{
    auto device = qsc::IDeviceManage::getInstance().getDevice(serial);

    if (action == "back") {
        if (device) {
            device->postGoBack();
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "BACK");
        }
    } else if (action == "home") {
        if (device) {
            device->postGoHome();
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "HOME");
        }
    } else if (action == "recent") {
        if (device) {
            device->postAppSwitch();
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "APP_SWITCH");
        }
    } else if (action == "power") {
        if (device) {
            device->postPower();
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "POWER");
        }
    } else if (action == "volume_down") {
        if (device) {
            device->postVolumeDown();
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "VOLUME_DOWN");
        }
    } else if (action == "volume_up") {
        if (device) {
            device->postVolumeUp();
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "VOLUME_UP");
        }
    } else if (action == "screen_on") {
        if (device) {
            device->setDisplayPower(true);
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "WAKEUP");
        }
    } else if (action == "screen_off") {
        if (device) {
            device->setDisplayPower(false);
        } else {
            runAdbShell(serial, QStringList() << "input" << "keyevent" << "SLEEP");
        }
    } else if (action == "screenshot") {
        if (device) {
            device->screenshot();
        } else {
            runAdbShell(serial, QStringList() << "screencap" << "-p" << QString("/sdcard/mphone-%1.png").arg(serial));
        }
    } else if (action == "group") {
        GroupController::instance().addDevice(serial);
    } else {
        outLog("unknown device action: " + action, false);
        return;
    }
    outLog(QString("device action: %1 -> %2").arg(serial, action), false);
}

void Dialog::on_wirelessDisConnectBtn_clicked()
{
    if (checkAdbRun()) {
        return;
    }
    QString addr = ui->deviceIpEdt->currentText().trimmed();
    outLog("wireless disconnect...", false);
    QStringList adbArgs;
    adbArgs << "disconnect";
    adbArgs << addr;
    m_adb.execute("", adbArgs);
}

void Dialog::on_selectRecordPathBtn_clicked()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this, tr("select path"), "", options);
    ui->recordPathEdt->setText(directory);
}

void Dialog::on_recordPathEdt_textChanged(const QString &arg1)
{
    ui->recordPathEdt->setToolTip(arg1.trimmed());
    ui->notDisplayCheck->setCheckable(!arg1.trimmed().isEmpty());
}

void Dialog::on_adbCommandBtn_clicked()
{
    execAdbCmd();
}

void Dialog::on_stopAdbBtn_clicked()
{
    m_adb.kill();
}

void Dialog::on_clearOut_clicked()
{
    ui->outEdit->clear();
}

void Dialog::on_stopAllServerBtn_clicked()
{
    qsc::IDeviceManage::getInstance().disconnectAllDevice();
}

void Dialog::on_recordScreenCheck_clicked(bool checked)
{
    if (!checked) {
        return;
    }

    QString fileDir(ui->recordPathEdt->text().trimmed());
    if (fileDir.isEmpty()) {
        qWarning() << "please select record save path!!!";
        ui->recordScreenCheck->setChecked(false);
    }
}

void Dialog::on_usbConnectBtn_clicked()
{
    on_stopAllServerBtn_clicked();
    delayMs(200);
    on_updateDevice_clicked();
    delayMs(200);
    startAllDeviceScreens();
}

int Dialog::findDeviceFromeSerialBox(bool wifi)
{
    QString regStr = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\:([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])\\b";
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QRegExp regIP(regStr);
#else
    QRegularExpression regIP(regStr);
#endif
    for (int i = 0; i < ui->serialBox->count(); ++i) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        bool isWifi = regIP.exactMatch(ui->serialBox->itemText(i));
#else
        bool isWifi = regIP.match(ui->serialBox->itemText(i)).hasMatch();
#endif
        bool found = wifi ? isWifi : !isWifi;
        if (found) {
            return i;
        }
    }

    return -1;
}

void Dialog::on_wifiConnectBtn_clicked()
{
    on_stopAllServerBtn_clicked();
    delayMs(200);

    on_updateDevice_clicked();
    delayMs(200);

    int firstUsbDevice = findDeviceFromeSerialBox(false);
    if (-1 == firstUsbDevice) {
        qWarning() << "No use device is found!";
        return;
    }
    ui->serialBox->setCurrentIndex(firstUsbDevice);

    on_getIPBtn_clicked();
    delayMs(200);

    on_startAdbdBtn_clicked();
    delayMs(1000);

    on_wirelessConnectBtn_clicked();
    delayMs(2000);

    on_updateDevice_clicked();
    delayMs(200);

    int firstWifiDevice = findDeviceFromeSerialBox(true);
    if (-1 == firstWifiDevice) {
        qWarning() << "No wifi device is found!";
        return;
    }
    ui->serialBox->setCurrentIndex(firstWifiDevice);

    on_startServerBtn_clicked();
}

void Dialog::on_connectedPhoneList_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    ui->serialBox->setCurrentIndex(ui->connectedPhoneList->currentRow());
    on_startServerBtn_clicked();
}

void Dialog::on_updateNameBtn_clicked()
{
    if (ui->serialBox->count() != 0) {
        if (ui->userNameEdt->text().isEmpty()) {
            Config::getInstance().setNickName(ui->serialBox->currentText(), "Phone");
        } else {
            Config::getInstance().setNickName(ui->serialBox->currentText(), ui->userNameEdt->text());
        }

        on_updateDevice_clicked();

        qDebug() << "Update OK!";
    } else {
        qWarning() << "No device is connected!";
    }
}

void Dialog::on_useSingleModeCheck_clicked()
{
    if (m_rightConfigScroll) {
        toggleRightPanel();
        return;
    }
}

void Dialog::on_serialBox_currentIndexChanged(const QString &arg1)
{
    ui->userNameEdt->setText(Config::getInstance().getNickName(arg1));
}

quint32 Dialog::getBitRate()
{
    return ui->bitRateEdit->text().trimmed().toUInt() *
            (ui->bitRateBox->currentText() == QString("Mbps") ? 1000000 : 1000);
}

const QString &Dialog::getServerPath()
{
    static QString serverPath;
    if (serverPath.isEmpty()) {
        serverPath = QString::fromLocal8Bit(qgetenv("MPHONE_SERVER_PATH"));
        QFileInfo fileInfo(serverPath);
        if (serverPath.isEmpty() || !fileInfo.isFile()) {
            serverPath = QCoreApplication::applicationDirPath() + "/scrcpy-server";
        }
    }
    return serverPath;
}

void Dialog::on_startAudioBtn_clicked()
{
    if (ui->serialBox->count() == 0) {
        qWarning() << "No device is connected!";
        return;
    }

    m_audioOutput.start(ui->serialBox->currentText(), 28200);
}

void Dialog::on_stopAudioBtn_clicked()
{
    m_audioOutput.stop();
}

void Dialog::on_installSndcpyBtn_clicked()
{
    if (ui->serialBox->count() == 0) {
        qWarning() << "No device is connected!";
        return;
    }
    m_audioOutput.installonly(ui->serialBox->currentText(), 28200);
}

void Dialog::on_autoUpdatecheckBox_toggled(bool checked)
{
    if (checked) {
        m_autoUpdatetimer.start(5000);
    } else {
        m_autoUpdatetimer.stop();
    }
}

void Dialog::loadIpHistory()
{
    QStringList ipList = Config::getInstance().getIpHistory();
    ui->deviceIpEdt->clear();
    ui->deviceIpEdt->addItems(ipList);
    ui->deviceIpEdt->setContentsMargins(0, 0, 0, 0);

    if (ui->deviceIpEdt->lineEdit()) {
        ui->deviceIpEdt->lineEdit()->setMaxLength(128);
        ui->deviceIpEdt->lineEdit()->setPlaceholderText("192.168.0.1");
    }
}

void Dialog::saveIpHistory(const QString &ip)
{
    if (ip.isEmpty()) {
        return;
    }
    
    Config::getInstance().saveIpHistory(ip);
    
    // 更新ComboBox
    loadIpHistory();
    ui->deviceIpEdt->setCurrentText(ip);
}

void Dialog::showIpEditMenu(const QPoint &pos)
{
    QMenu *menu = ui->deviceIpEdt->lineEdit()->createStandardContextMenu();
    menu->addSeparator();
    
    QAction *clearHistoryAction = new QAction(tr("Clear History"), menu);
    connect(clearHistoryAction, &QAction::triggered, this, [this]() {
        Config::getInstance().clearIpHistory();
        loadIpHistory();
    });
    
    menu->addAction(clearHistoryAction);
    menu->exec(ui->deviceIpEdt->lineEdit()->mapToGlobal(pos));
    delete menu;
}

void Dialog::loadPortHistory()
{
    QStringList portList = Config::getInstance().getPortHistory();
    ui->devicePortEdt->clear();
    ui->devicePortEdt->addItems(portList);
    ui->devicePortEdt->setContentsMargins(0, 0, 0, 0);

    if (ui->devicePortEdt->lineEdit()) {
        ui->devicePortEdt->lineEdit()->setMaxLength(6);
        ui->devicePortEdt->lineEdit()->setPlaceholderText("5555");
    }
}

void Dialog::savePortHistory(const QString &port)
{
    if (port.isEmpty()) {
        return;
    }
    
    Config::getInstance().savePortHistory(port);
    
    // 更新ComboBox
    loadPortHistory();
    ui->devicePortEdt->setCurrentText(port);
}

void Dialog::showPortEditMenu(const QPoint &pos)
{
    QMenu *menu = ui->devicePortEdt->lineEdit()->createStandardContextMenu();
    menu->addSeparator();
    
    QAction *clearHistoryAction = new QAction(tr("Clear History"), menu);
    connect(clearHistoryAction, &QAction::triggered, this, [this]() {
        Config::getInstance().clearPortHistory();
        loadPortHistory();
    });
    
    menu->addAction(clearHistoryAction);
    menu->exec(ui->devicePortEdt->lineEdit()->mapToGlobal(pos));
    delete menu;
}
