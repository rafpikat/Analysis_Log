#include <stdio.h>   // Thư viện với các hàm nhập xuất đã được định nghĩa
#include <string.h>  // Thư viện với các hàm và các macro để làm việc với chuỗi kí tự
#include <stdbool.h> // Thư viện với các định nghĩa và macro để hỗ trợ kiểu dữ liệu boolean
#include <stdlib.h>  // Thư viện liên quan đến các hoạt động thực thi hệ thống và quản lý bộ nhớ (ví dụ: exit(), system(), malloc(), calloc(), realloc(), free(), strtod)

#define FILENAME "log.txt" // Tên file trong cùng thư mục
#define MAXLENGTHFILE 5000 // Độ dài tối đa của chuỗi đọc từ file

char fileStr[MAXLENGTHFILE]; // Lưu trữ chuỗi trong file log.txt
char thuocTinh[18][27][50];  // Lưu trữ các thuộc tính đã được tách bởi strtok (Ví dụ: INFO, cmd, set, objects, type, ...)

int fileToStr(char *str);                                             // Lưu trữ chuỗi trong file log.txt vào mảng fileStr
void soBanTinGuiDi(char *str);                                        // Hàm tính số bản tin gửi đi
void soBanTinGuiDenThietBi(char *str);                                // Đếm bản tin gửi đi với mã network của thiết bị thực hiện truyền nhận dữ liệu
void tachThuocTinh(char *str);                                        // Tách các thuộc tính trong mỗi bản tin (Ví dụ: INFO, cmd, set, objects, type, ...) và lưu vào mảng thuocTinh
bool ktChuoiLap(char *chuoi1, char *chuoi2, char netEndp[18][2][25]); // Kiểm tra chuỗi lặp trong mảng netEndp
void soCongTac();                                                     // Thực hiện đếm số địa chỉ network và endpoint có trong danh sách các bản tin
char *reqidBanTin(char *chuoi, char banTin[27][50]);                  // Lấy reqId của bản tin set hoặc status tùy vào đối số chuỗi đưa vào là set hay status
void soBanTinGuiLoi();                                                // Tính số bản tin lỗi
void thoiGianTreLonNhat();                                            // Tính độ trễ lớn nhất giữa bản tin gửi đi và bản tin phản hồi
void thoiGianTreTrungBinh();                                          // Tính thời gian trễ trung bình trong khoảng thời gian log

int main()
{
    fileToStr(fileStr);          // Đọc file log.txt và lưu chuỗi vào mảng fileStr
    char tempLog[MAXLENGTHFILE]; // Tạo một mảng tạm để tránh các hàm sử dụng strtok làm thay đổi chuỗi gốc
    strcpy(tempLog, fileStr);    // Copy thông tin chuỗi trong mảng fileStr vào mảng tempLog

    // Câu 1: Tính số bản tin gửi đi
    soBanTinGuiDi(tempLog);

    // Câu 2: Tính số bản tin gửi đi theo mã network của thiết bị
    strcpy(tempLog, fileStr);
    soBanTinGuiDenThietBi(tempLog);

    // Câu 3: Tính số công tắc có trao đổi thông tin với bộ điều khiển trung tâm
    strcpy(tempLog, fileStr);
    tachThuocTinh(tempLog);
    soCongTac();

    // Câu 4: Tính số bản tin gửi đi bị lỗi
    soBanTinGuiLoi();

    // Câu 4: Tính độ trễ lớn nhất giữa bản tin gửi đi và bản tin phản hồi
    thoiGianTreLonNhat();

    // Câu 5: Tính thời gian trễ trung bình trong khoảng thời gian log
    thoiGianTreTrungBinh();

    return 0;
}

int fileToStr(char *str) // Hàm có sẵn giúp đọc file log.txt và lưu chuỗi vào mảng fileStr
{
    int status;
    FILE *fp = NULL;

    fp = fopen(FILENAME, "r"); // Mở tệp tin FILENAME để đọc
    if (fp == NULL)            // Nếu fp = NULL thì việc mở tệp tin không thành công
    {
        printf("File does not exist\n");
        return -1;
    }
    status = fread(str, 1, MAXLENGTHFILE, fp); // Đọc dữ liệu từ tệp tin và lưu trữ vào str

    fclose(fp);
    fp = NULL;
    return status;
}

void soBanTinGuiDi(char *str)
{
    int count = 0;
    char *token = strtok(str, "\n"); // Tách bản tin đầu tiên trong chuỗi str bởi ký tự xuống dòng

    while (token != NULL) // Chạy cho đến khi kết thúc chuỗi (token == NULL)
    {
        if (strstr(token, "\"cmd\":\"set\"") != NULL) // Tìm chuỗi "\"cmd\":\"set\"" trong token bằng hàm strstr. Khác NULL là tìm thấy
        {
            count++;               // tìm thấy chuỗi "\"cmd\":\"set\"" thì biến đếm cộng 1
            printf("\n%s", token); // In ra chuỗi có chứa "\"cmd\":\"set\""
        }
        token = strtok(NULL, "\n"); // Tách các bản tin tiếp theo trong chuỗi str bởi ký tự xuống dòng
    }
    printf("\nSo ban tin = %d", count); // In ra số lượng bản tin đã gửi đi
    printf("\n-------------------------------------");
}

void soBanTinGuiDenThietBi(char *str)
{
    char tempStr[MAXLENGTHFILE];
    strcpy(tempStr, str);                // Tạo một bản sao của str để khi sử dụng hàm token không bị thay đổi chuỗi str gốc
    char *token = strtok(tempStr, "\n"); // Tách bản tin đầu tiên trong chuỗi tempStr bởi ký tự xuống dòng
    char netWork[100];
    int count = 0;

    while (1)
    {
        printf("\nNhap ma network: ");
        scanf("%s", netWork); // Nhập vào mã network của thiết bị muốn tìm kiếm bản tin gửi đi

        if (strstr(str, netWork) == NULL) // Tìm kiếm chuỗi network trong toàn bộ chuỗi str. Nếu không sao chép thì hàm strstr chỉ tìm kiếm trong bản tin đầu tiên của chuỗi str do hàm strtok
        {
            printf("Ma network khong hop le. Nhap lai");
        }
        else
        {
            while (token != NULL)
            {
                if (strstr(token, "\"cmd\":\"set\"") != NULL && strstr(token, netWork) != NULL) // Tìm kiếm chuỗi con "\"cmd\":\"set\"" và chuỗi con network trong từng bản tin token
                {
                    count++;
                    printf("\n%s", token);
                }
                token = strtok(NULL, "\n"); // Tiếp tục tách các bản tin tiếp theo trong chuỗi tempStr bởi ký tự xuống dòng
            }
            break; // Thoát khỏi vòng lặp khi đã thực hiện xong đếm số bản tin gửi đi theo mã network
        }
    }
    printf("\nSo ban tin gui di la: %d", count);
    printf("\n-------------------------------------");
}

void tachThuocTinh(char *str)
{
    char *tokenThuocTinh = strtok(str, "\n,[]\":{}- "); // Tách các thuộc tính bằng các ký tự "\n,[]\":{}- " trong toàn bộ chuỗi str

    for (int i = 0; i < 18; i++) // Chạy vòng lặp với 18 bản tin
    {
        if (i % 2 == 0) // Với các bản tin số chẵn thì sẽ là bản tin set
        {
            for (int j = 0; j < 26; j++) // Bản tin set có 26 thuộc tính
            {
                strcpy(thuocTinh[i][j], tokenThuocTinh); // Lưu thông tin thuộc tính vào mảng thuocTinh
                tokenThuocTinh = strtok(NULL, "\n,[]\":{}- ");
            }
            strcpy(thuocTinh[i][26], ""); // Ký tự cuối cùng trong bản tin set sẽ là ký tự null
        }
        else // Với các bản tin số lẻ thì sẽ là bản tin status
        {
            for (int j = 0; j < 27; j++) // Bản tin status có 27 thuộc tính
            {
                strcpy(thuocTinh[i][j], tokenThuocTinh); // Lưu thông tin thuộc tính vào mảng thuocTinh
                tokenThuocTinh = strtok(NULL, "\n,[]\":{}- ");
            }
        }
    }
}

void soCongTac()
{
    char netEndp[18][2][25]; // Lưu trữ các địa chỉ network và endpoint không trùng lặp với tối đa 18 bản tin, 2 thuộc tính network và endpoint và độ dài chuỗi không quá 25 ký tự
    int indexNE = 0;
    for (int i = 0; i < 18; i++) // Chạy vòng lặp với 18 bản tin trong mảng thuocTinh
    {
        for (int j = 0; j < 27; j++) // Chạy vòng lặp với từng thuộc tính trong mảng thuocTinh
        {
            if (!strcmp(thuocTinh[i][j], "cmd") && strcmp(thuocTinh[i][j + 1], "set")) // Nếu bản tin có thuộc tính cmd nhưng không có thuộc tính set thì break vòng lặp (bỏ qua bản tin đó)
                break;
            if (!strcmp(thuocTinh[i][j], "type") && !strcmp(thuocTinh[i][j + 1], "switch")) // Tìm kiếm các bản tin có thuộc tính type và switch
            {
                if (!ktChuoiLap(thuocTinh[i][j + 4], thuocTinh[i][j + 6], netEndp)) // Kiểm tra thuộc tính có nằm trong mảng netEndp không?
                {
                    strcpy(netEndp[indexNE][0], thuocTinh[i][j + 4]); // Lưu thuộc tính network vào mảng
                    strcpy(netEndp[indexNE][1], thuocTinh[i][j + 6]); // Lưu thuộc tính endpoint vào mảng
                    indexNE++;
                }
                break;
            }
        }
    }
    for (int i = 0; i < indexNE; i++) // In số địa chỉ network và endpoint trong danh sách không trùng lặp
    {
        printf("\nThiet bi %d co dia chi la : NWK - %s, ENDPOINT - %s", i + 1, netEndp[i][0], netEndp[i][1]);
    }
    printf("\n-------------------------------------");
}

bool ktChuoiLap(char *chuoi1, char *chuoi2, char netEndp[18][2][25])
{
    for (int i = 0; i < 18; i++)
    {
        if (strcmp(netEndp[i][0], chuoi1) == 0 && strcmp(netEndp[i][1], chuoi2) == 0) // nếu chuoi 1 và chuoi 2 nam trong mảng netEndp thì trả về true
            return true;
    }
    return false;
}

char *reqidBanTin(char *chuoi, char banTin[27][50])
{
    int soHang;
    soHang = (chuoi, "set") ? 26 : 27; // nếu chuỗi đưa vào là set thì số hàng sẽ là 26, ngược lại sẽ là 27
    for (int i = 0; i < soHang; i++)   // Chạy vòng lặp với toàn bộ các thuộc tính trong một bản tin
    {
        if (!strcmp(banTin[i], chuoi)) // nếu không tìm được chuỗi trong bản tin thì continue
            continue;

        if (strcmp(banTin[i], "reqid") == 0)
        {
            return banTin[i + 1]; // Lấy req id của bản tin chứa chuỗi
        }
    }
}

void soBanTinGuiLoi()
{
    int countBanTinLoi = 0;
    for (int i = 0; i < 18; i += 2)
    {
        if (strcmp(reqidBanTin("set", thuocTinh[i]), reqidBanTin("status", thuocTinh[i + 1])) != 0) // Nếu reqid của 2 bản tin là set và status không bằng nhau thì số bản tin lỗi tăng lên 1
            countBanTinLoi++;
    }

    printf("\nSo ban tin loi: %d", countBanTinLoi);
    printf("\n-------------------------------------");
}

void thoiGianTreLonNhat()
{
    double maxDelay = 0, setTime, statusTime;
    for (int i = 0; i < 18; i += 2)
    {
        if (strcmp(reqidBanTin("set", thuocTinh[i]), reqidBanTin("status", thuocTinh[i + 1])) == 0) // Tìm kiếm các bản tin set và status có reqid bằng nhau
        {
            setTime = strtod(thuocTinh[i][3] + 2, NULL) * 60 * 60 * 1000 + strtod(thuocTinh[i][4], NULL) * 60 * 1000 + strtod(thuocTinh[i][5], NULL) * 1000;                // Thời gian gửi bản tin set, sử dụng strtod để chuyển đổi chuỗi thành số thực kiểu double
            statusTime = strtod(thuocTinh[i + 1][3] + 2, NULL) * 60 * 60 * 1000 + strtod(thuocTinh[i + 1][4], NULL) * 60 * 1000 + strtod(thuocTinh[i + 1][5], NULL) * 1000; // Thời gian gửi bản tin status, sử dụng strtod để chuyển đổi chuỗi thành số thực kiểu double
            maxDelay = (maxDelay < statusTime - setTime) ? statusTime - setTime : maxDelay;                                                                                 // Tìm độ trễ lớn nhất
        }
    }
    printf("\nDo tre lon nhat la: %.lf Millisecond", maxDelay);
    printf("\n-------------------------------------");
}

void thoiGianTreTrungBinh()
{
    double trungBinh = 0, setTime, statusTime, count = 0;
    for (int i = 0; i < 18; i += 2)
    {
        if (strcmp(reqidBanTin("set", thuocTinh[i]), reqidBanTin("status", thuocTinh[i + 1])) == 0) // Tìm kiếm các bản tin set và status có reqid bằng nhau
        {
            setTime = strtod(thuocTinh[i][3] + 2, NULL) * 60 * 60 * 1000 + strtod(thuocTinh[i][4], NULL) * 60 * 1000 + strtod(thuocTinh[i][5], NULL) * 1000;                // Thời gian gửi bản tin set, sử dụng strtod để chuyển đổi chuỗi thành số thực kiểu double
            statusTime = strtod(thuocTinh[i + 1][3] + 2, NULL) * 60 * 60 * 1000 + strtod(thuocTinh[i + 1][4], NULL) * 60 * 1000 + strtod(thuocTinh[i + 1][5], NULL) * 1000; // Thời gian gửi bản tin status, sử dụng strtod để chuyển đổi chuỗi thành số thực kiểu double
            trungBinh += statusTime - setTime;
            count++;
        }
    }
    printf("\nDo tre trung binh la: %.lf Millisecond", trungBinh / count);
    printf("\n-------------------------------------");
}