#pragma once

//---------------------------------------------------------------------------------
//	Float2 クラス
//---------------------------------------------------------------------------------
class Float2
{
public:
	float x;
	float y;

	Float2();									//	コンストラクタ
	Float2( float x, float y );

	void Clear();								//	ゼロ初期化
	void Set( float x, float y );				//	値のセット
	void Set( Float2 &f2 );

	Float2& operator = ( const Float2 &f2 );	//	= 演算子のオーバーロード

	Float2& operator += ( const Float2 &f2 );	//	+= 演算子のオーバーロード
	Float2& operator -= ( const Float2 &f2 );	//	-= 演算子のオーバーロード
	Float2& operator *= ( const float f );		//	*= 演算子のオーバーロード
	Float2& operator /= ( const float f );		//	/= 演算子のオーバーロード
};

//	+ 演算子のオーバーロード
Float2 operator + ( const Float2 &f2_1, const Float2 &f2_2 );
//	- 演算子のオーバーロード
Float2 operator - ( const Float2 &f2_1, const Float2 &f2_2 );
//	* 演算子のオーバーロード
Float2 operator * ( const Float2 &f2, const float f );
//	/ 演算子のオーバーロード
Float2 operator / ( const Float2 &f2, const float f );

//	マウス座標を Float2 で取得
Float2 GetMouseFloat2();
//	２つの Float2 の距離を求める
float GetFloat2Distance( Float2 &pos1, Float2 &pos2 );
