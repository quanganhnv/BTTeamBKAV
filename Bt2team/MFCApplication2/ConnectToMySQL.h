#pragma once
#include<mysql.h>
#pragma warning(disable : 4996)

MYSQL* g_mySQL = mysql_init(0);
MYSQL_RES* selectAllQuery() {
	MYSQL_RES* result = 0; 
	int iResult = 0;
	iResult = mysql_query(g_mySQL, "SELECT * FROM tb_nhanvien");
	if (iResult != 0)		//câu lệnh thực hiện thành công thì iResult = 0
	{
		MessageBoxA(0, mysql_error(g_mySQL), "Error", 0);
		mysql_close(g_mySQL);
		return result;
	}

	//Kiểm tra kết quả trả về của query sau khi thực hiện
	result = mysql_store_result(g_mySQL);
	if (result == NULL) {	//Câu lệnh thực hiện thành công thì != NULL
		MessageBoxA(0, mysql_error(g_mySQL), "Loi", 0);
		mysql_close(g_mySQL);
		return result;
	}
	return result;
}

MYSQL_RES* selectOnePersonQuery(char* ID) {
    MYSQL_RES* result = 0;
    char query[512] = { 0 };
    sprintf(query, "SELECT * FROM tb_nhanvien WHERE ID = %s", ID);  // Chọn toàn bộ thông tin nhân viên có ID là : ID
    if (mysql_query(g_mySQL,query)) {
        MessageBoxA(0, mysql_error(g_mySQL), "Loi", 0);
        mysql_close(g_mySQL);
        return result;
    }
    result = mysql_store_result(g_mySQL);
    if (result == NULL) {
        MessageBoxA(0, mysql_error(g_mySQL), "Loi", 0);
        mysql_close(g_mySQL);
        return result;
    }
    return result;
}

bool Query(char* query) {
	if (mysql_query(g_mySQL, query))	//lệnh thực hiện thành công thì trả về 0
		return FALSE;
	else
		return TRUE;
}

int ConnectToSQL() {
    if (g_mySQL == NULL) {
        MessageBoxA(0, mysql_error(g_mySQL), "Loi", 0);
        return 0;
    }

    if (mysql_real_connect(g_mySQL, "localhost", "root", "Qa06052001", "NhanVien", 3306, 0, 0) == NULL) {       //connect tới MySQL server local
        MessageBoxA(0, mysql_error(g_mySQL), "Loi", 0);
        mysql_close(g_mySQL);
        return 0;
    }

    // Create table tb_nhanvien
    if (mysql_query(g_mySQL, "CREATE TABLE tb_nhanvien (ID INT AUTO_INCREMENT PRIMARY KEY,Account text,HoTen text,QueQuan text,NgaySinh date,GioiTinh int,TruongHoc text)"))    //Nếu Lỗi
    {
        MessageBoxA(0, mysql_error(g_mySQL), "Thong Bao", 0);
        CONST CHAR* ColumnName[] = { "ID", "Account","Hoten","QueQuan","NgaySinh","GioiTinh","TruongHoc" };
        CHAR Query[512] = { 0 };

        if (mysql_errno(g_mySQL) == 1050)           //mysql_errno : lỗi gần nhất.Nếu Lỗi là bảng đã tồn tại 
        {
            for (int i = 0; i < 7; i++)
            {
                ZeroMemory(Query, sizeof(Query));
                if (i == 0)
                    continue;
                else if (i == 5)
                    sprintf(Query, "ALTER TABLE tb_nhanvien ADD COLUMN %s INT", ColumnName[5]);     //Cột giới tính phải là cột INT
                else
                    sprintf(Query, "ALTER TABLE tb_nhanvien ADD COLUMN %s TEXT", ColumnName[i]);
                mysql_query(g_mySQL, Query); // bổ sung cột nếu bảng đã tồn tại
            }
        }
    }
    return 1;

    
}


