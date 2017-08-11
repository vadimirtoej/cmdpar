#include <iostream>
#include <chrono>
#include <ctime>

void calc(double* n);

//Программка демонстрирующая одновременное выполнение нескольких опеераций процессором, если их для выполнения не нужен результат другой операции

int main(int argc, char** argv)
{
	namespace C = std::chrono;
	const size_t iter_count = 1000000000;
	std::srand(std::time(nullptr));
	//Первый тест: мы умножаем число n - раз
	{
		const auto start = C::steady_clock::now();
		double n = rand() % 1000;
		for (size_t i = 0; i < iter_count; ++i) {
			n *= 2;
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		std::cout << "test#1: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		//вывод результата нужен, чтобы при оптимизации кода O3 умножение числа не было выкинуто, т.к. далее оно нигде не использовалось
		std::cout << "the result is " << n << "\n";
	}
	//Второй тест: мы умножаем число 2*n - раз
	{
		const auto start = C::steady_clock::now();
		double n = rand() % 1000;
		for (size_t i = 0; i < iter_count*2; ++i) {
			n *= 2;
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		std::cout << "test#2: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		std::cout << "the result is " << n << "\n";
	}
	//Третий тест: мы умножаем независимо друг от друга 3 числа n раз
	{
		const auto start = C::steady_clock::now();
		double n = rand() % 1000;
		double n2 = rand() % 1000;
		double n3 = rand() % 1000;
		for (size_t i = 0; i < iter_count; ++i) {
			n *= 2;
			n2 *= 3;
			n3 *= 4;
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		std::cout << "test#3: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		std::cout << "the result is " << n << "_" << n2 << "_" << n3 << "\n";
	}
	// Четвёртый тест: у нас есть статический массив из 3х элементов, перемножаем умножаем каждое из чисел на заданное n раз.
	// При этом обход элементов в массиве тоже делаем в цикле.
	// Этот тест должен показать как у нас происходит "раскрутка" цикла (loop unrolling) в простую последовательность операций,
	// которые при этом ещё выполняются одновременно
	{
		const auto start = C::steady_clock::now();
		const size_t el_count = 3;
		double n[el_count];
		for(size_t i = 0; i < el_count; ++i){
			n[i] = rand() % 1000;			
		}
		for (size_t i = 0; i < iter_count; ++i) {
			for(size_t j = 0; j < el_count; ++j){
				n[j] *= 3;
			}
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		double res = 0;
		for(size_t j = 0; j < el_count; ++j){
			res += n[j];
		}
		std::cout << "test#4: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		std::cout << "the result is " << res << "\n";
	}
	// Пятый тест: похож на четвёртый, но данные храняться в динамическом массиве и его размер НЕ изветен на этапе компиляции.
	// Здесь не удаётся раскрутить цикл
	{
		if(argc < 2){
			std::cout << "spec el count\n";
			return -1;
		}
		const size_t el_count = atoi(argv[1]);
		const auto start = C::steady_clock::now();
		double* const n = new double[el_count];
		for(size_t i = 0; i < el_count; ++i){
			n[i] = rand() % 1000;			
		}

		for (size_t i = 0; i < iter_count; ++i) {
			for(size_t j = 0; j < el_count; ++j){
				n[j] *= 3;
			}
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		double res = 0;
		for(size_t j = 0; j < el_count; ++j){
			res += n[j];
		}
		std::cout << "test#5: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		std::cout << "the result is " << res << "\n";
		delete[] n;
	}
	// Шестой тест: похож на четвёртый, но данные храняться в динамическом массиве и его размер изветен на этапе компиляции.
	// Здесь УДАЁТСЯ раскрутить цикл и результат теста должен быть похож на результат 4ого теста
	{
		if(argc < 2){
			std::cout << "spec el count\n";
			return -1;
		}
		const size_t el_count = 3;
		const auto start = C::steady_clock::now();
		double* const n = new double[el_count];
		for(size_t i = 0; i < el_count; ++i){
			n[i] = rand() % 1000;			
		}
		for (size_t i = 0; i < iter_count; ++i) {
			for(size_t j = 0; j < el_count; ++j){
				n[j] *= 3;
			}
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		double res = 0;
		for(size_t j = 0; j < el_count; ++j){
			res += n[j];
		}
		std::cout << "test#6: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		std::cout << "the result is " << res << "\n";
		delete[] n;
	}
	// Седьмой тест: такой же как пятый, но умножением элементов происходит в отдельной функции calc. В ней происходит раскрутка цикла,
	// т.к. количество элементов массива, которое нужно обработать известно на этапе компиляции
	{
		if(argc < 2){
			std::cout << "spec el count\n";
			return -1;
		}
		const size_t el_count = atoi(argv[1]);
		const auto start = C::steady_clock::now();
		double* const n = new double[el_count];
		for(size_t i = 0; i < el_count; ++i){
			n[i] = rand() % 1000;			
		}
		for (size_t i = 0; i < iter_count; ++i) {
			calc(n);
		}
		const auto end = C::steady_clock::now();
		const auto delta = end - start;
		double res = 0;
		for(size_t j = 0; j < el_count; ++j){
			res += n[j];
		}
		std::cout << "test#7: " << C::duration_cast<C::microseconds>(delta).count() << " us\n";
		std::cout << "the result is " << res << "\n";
		delete[] n;
	}
	return 0;
}


void calc(double* n)
{
	const size_t el_count = 3;
	for(size_t j = 0; j < el_count; ++j){
		n[j] *= 3;
	}	
}
