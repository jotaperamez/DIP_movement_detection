#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>
#include <iostream>
#include <string>
#include <stdio.h>

int preprocess(C_Image in);
int size_diff(C_Image in_base, C_Image in_diff);
void segmentation(C_Image in_base, C_Image in_diff, int max, int min, double base_pp, double diff_pp, int reduce, int expand);
C_Image dip_erosion(C_Image in_base, int in_erosion);
C_Image dip_dilatation(C_Image in_erosed);

using namespace std;

int main(int argc, char **argv) {
	C_Image in_base, in_diff, out;
	string in_base_file, in_diff_file;
	int dilatation = 2;
	int erosion = 2;
	bool default_values;

	/*if (argv[1] != NULL && argv[2] != NULL) {
		in_base.ReadBMP(argv[1]);
		in_diff.ReadBMP(argv[2]);
	}
	if (argv[3] != NULL && argv[4] != NULL) {
		dilatation = (int)argv[3];
		erosion = (int)argv[4];
	}*/
	
	cout << "\t***************************************\n";
	cout << "\t** SEGMENTACION BASADA EN MOVIMIENTO **\n";
	cout << "\t** --------------------------------- **\n";
	cout << "\t**      Juan Pedro Ramos Gomez       **\n";
	cout << "\t***************************************\n\n\n";

	cout << "Nombre completo o ruta absoluta de la primera imagen [bmp]: " << endl;
	cin >> in_base_file;
	cout << "Nombre completo o ruta absoluta de la segunda imagen [bmp]: " << endl;
	cin >> in_diff_file;

	in_base.ReadBMP(in_base_file.c_str());
	in_diff.ReadBMP(in_diff_file.c_str());
	out.ReadBMP(in_base_file.c_str());

	cout << "\n>> Archivos " << in_base_file << " y " << in_diff_file << " cargados correctamente.\n\n";

	cout << "Los valores por defecto para" << endl;
	cout << "la erosion y dilatacion de los pixeles" << endl;
	cout << "negros y blancos se ha establecido a '2' por defecto." << endl;
	cout << "¿ Utilizar valores por defecto ? (1: Si, 0: No)" << endl;
	cin >> default_values;
	if (!default_values) {
		cout << "Nuevo valor para 'Erosion': " << endl;
		cin >> erosion;
		cout << "Nuevo valor para 'Dilatacion': " << endl;
		cin >> dilatation;
	}

	cout << "\nEstableciendo valores, medias, niveles de gris...\n";
	double in_base_pp = preprocess(in_base);
	double in_diff_pp = preprocess(in_diff);
	int max = size_diff(in_base, in_diff);
	int min = in_base_pp - in_diff_pp;
	cout << "Valores establecidos.\n";

	cout << "Segmentando ... \n";
	segmentation(in_base, in_diff, max, min, in_base_pp, in_diff_pp, erosion, dilatation);
	cout << "\n Segmentacion realizada con exito. Se han generado los archivos\n en el directorio /Run de su proyecto.";

	return 0;
}

#pragma region BLACK MAGIC
int preprocess(C_Image in) {
	C_Image::IndexT row, col;
	int sum = 0;
	int size = 0;

	for (row = in.FirstRow(); row <= in.LastRow(); row++) {
		for (col = in.FirstCol(); col <= in.LastCol(); col++) {
			sum = sum + in(row, col);
			size++;
		}
	}

	return sum / size;
}
int size_diff(C_Image in_base, C_Image in_diff) {
	C_Image::IndexT row, col;
	int data[11];

	for (row = in_base.FirstRow(); row <= in_base.LastRow(); row++) {
		for (col = in_base.FirstCol(); col <= in_base.LastCol(); col++) {

			int diff = in_base(row, col) - in_diff(row, col);
			if (abs(diff) == 0) {}
			else if (diff > 0 && diff <= 10) {
				data[0]++;
			}
			else if (diff > 10 && diff <= 20) {
				data[1]++;
			}
			else if (diff > 20 && diff <= 30) {
				data[2]++;
			}
			else if (diff > 30 && diff <= 40) {
				data[3]++;
			}
			else if (diff > 40 && diff <= 50) {
				data[4]++;
			}
			else if (diff > 50 && diff <= 60) {
				data[5]++;
			}
			else if (diff > 60 && diff <= 70) {
				data[6]++;
			}
			else if (diff > 70 && diff <= 80) {
				data[7]++;
			}
			else if (diff > 80 && diff <= 90) {
				data[8]++;
			}
			else if (diff > 90 && diff <= 100) {
				data[9]++;
			}
			else {
				data[10]++;
			}
		}
	}

	int sum = 0;
	int size_diff = 10;

	for (int i = 1; i < 10; i++) {
		sum += data[i];
	}
	sum /= 8;

	for (int i = 1; i < 10; i++) {
		size_diff += 10;
		if (data[i] < sum) {
			break;
		}
	}
	return size_diff;
}
void segmentation(C_Image in_base, C_Image in_diff, int max, int min, double base_pp, double diff_pp, int reduce, int expand) {
	C_Image::IndexT row, col;
	C_Image out = in_base;

	double div = 0;

	div = diff_pp / base_pp;
	if (base_pp < diff_pp) {
		div = base_pp / diff_pp;
	}

	row, col = 0;
	for (row = in_base.FirstRow(); row <= in_base.LastRow(); row++) {
		for (col = in_base.FirstCol(); col <= in_base.LastCol(); col++) {
			if (min < 0) {
				in_diff(row, col) = in_diff(row, col) * div;
				if (in_diff(row, col) < 0) {
					in_diff(row, col) = 0;
				}

			}
			else {
				in_diff(row, col) = in_diff(row, col) / div;
				if (in_diff(row, col) > 255) {
					in_diff(row, col) = 255;
				}
			}
		}
	}
	in_diff.WriteBMP("OUT_DILATATION.BMP");

	for (row = in_base.FirstRow(); row <= in_base.LastRow(); row++) {
		for (col = in_base.FirstCol(); col <= in_base.LastCol(); col++) {
			if (in_base(row, col) - in_diff(row, col) >= max || in_diff(row, col) - in_base(row, col) >= max) {
				out(row, col) = 255;
			}
			else {
				out(row, col) = 0;
			}
		}
	}

	C_Image reducctio;
	C_Image expandio;
	for (int i = 0; i < reduce; i++)
	{
		reducctio = dip_erosion(out, 0);
		out = reducctio;
	}
	reducctio.WriteBMP("OUT_EROSION.bmp");
	for (int i = 0; i < expand; i++)
	{
		expandio = dip_dilatation(reducctio);
		reducctio = expandio;
	}
	expandio.WriteBMP("OUT_FINAL.bmp");
}
C_Image dip_erosion(C_Image in_base, int in_erosion) {
	C_Image::IndexT row, col;
	C_Image out_erosion = in_base;

	if (in_erosion > 0) {
		for (int i = 0; i < 256; i++) {
			for (row = in_base.FirstRow(); row <= in_base.LastRow(); row++) {
				for (col = in_base.FirstCol(); col <= in_base.LastCol(); col++) {
					if (in_base(row, col) == in_erosion) {
						if (row > in_base.FirstRow()) {
							out_erosion(row - 1, col) = in_erosion;
						}
						if (row < in_base.LastRow()) {
							out_erosion(row + 1, col) = in_erosion;
						}
						if (col > in_base.FirstCol()) {
							out_erosion(row, col - 1) = in_erosion;
						}
						if (col < in_base.LastCol()) {
							out_erosion(row, col + 1) = in_erosion;
						}
					}
				}
				in_erosion++;
			}
		}
	}
	else {
		in_erosion = 0;
		for (row = in_base.FirstRow(); row <= in_base.LastRow(); row++) {
			for (col = in_base.FirstCol(); col <= in_base.LastCol(); col++) {
				if (in_base(row, col) == in_erosion) {
					if (row > in_base.FirstRow()) {
						out_erosion(row - 1, col) = in_erosion;
					}
					if (row < in_base.LastRow()) {
						out_erosion(row + 1, col) = in_erosion;
					}
					if (col > in_base.FirstCol()) {
						out_erosion(row, col - 1) = in_erosion;
					}
					if (col < in_base.LastCol()) {
						out_erosion(row, col + 1) = in_erosion;
					}
				}
			}
		}
		return out_erosion;
	}
	return out_erosion;
}
C_Image dip_dilatation(C_Image in_erosed) {
	C_Image::IndexT row, col;
	C_Image out_dilatation = in_erosed;

	for (row = in_erosed.FirstRow(); row <= in_erosed.LastRow(); row++) {
		for (col = in_erosed.FirstCol(); col <= in_erosed.LastCol(); col++) {
			if (in_erosed(row, col) == 255) {
				if (row > in_erosed.FirstRow()) {
					out_dilatation(row - 1, col) = 255;
				}
				if (row < in_erosed.LastRow()) {
					out_dilatation(row + 1, col) = 255;
				}
				if (col > in_erosed.FirstCol()) {
					out_dilatation(row, col - 1) = 255;
				}
				if (col < in_erosed.LastCol()) {
					out_dilatation(row, col + 1) = 255;
				}
			}
		}
	}
	return out_dilatation;
}
#pragma endregion


