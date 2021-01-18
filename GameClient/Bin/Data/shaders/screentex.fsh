varying vec2 v_texCoord;	//	2���� �ؽ��� ��ǥ�� ��Ÿ���� ���͸� �����Ѵ�.
varying vec4 colorVarying;	//	4���� ������ ��Ÿ���� ���͸� �����Ѵ�.

uniform sampler2D Texture;	//	2���� ���÷� ������ �����Ѵ�.
uniform vec4 ColorMulti;	//	4���� ��Ƽ ���� ��Ÿ���� ���͸� �����Ѵ�.
uniform vec4 ColorAdd;	//	4���� �� �߰� ������ �����Ѵ�.

uniform vec4 TopCol;	//	4���� ��� �浹�� ��Ÿ���� ���͸� �����Ѵ�.
uniform vec4 BotCol;	//	4���� �ϴ� �浹�� ��Ÿ���� ���͸� �����Ѵ�.

uniform vec2 fragSize;	//	2���� �ȼ� ũ�⸦ ��Ÿ���� ���͸� �����Ѵ�.

void main()	//	�ȼ����̴��� ���� �Լ��� �����Ѵ�.
{
	float invColorMultiAlpha = 1.0 - ColorMulti.a;	//	float ������ �����ϰ� 1���� ��Ƽ ���� ���İ��� �������� �����Ѵ�.
    	gl_FragColor = colorVarying * texture2D(Texture, v_texCoord);	//	�ȼ� ���� ����� 2���� ���÷��� �ؽ��� ��ǥ�� ���ø��� ���� ���� ������ �����Ѵ�.

	gl_FragColor.r = ( ColorMulti.r * ColorMulti.a ) + ( invColorMultiAlpha ) * gl_FragColor.r;	//	�ȼ� ���� r ������ ��Ƽ ���� r���а� ��Ƽ ���� a������ ���ѰͰ� 
	gl_FragColor.g = ( ColorMulti.g * ColorMulti.a ) + ( invColorMultiAlpha ) * gl_FragColor.g;
	gl_FragColor.b = ( ColorMulti.b * ColorMulti.a ) + ( invColorMultiAlpha ) * gl_FragColor.b;

	float percent = gl_FragCoord.y / fragSize.y;
	float invPercent = 1.0 - percent;
	vec4 gradColor;
	
	
	gradColor.r = ( TopCol.r * percent + BotCol.r * invPercent ) * invColorMultiAlpha;
	gradColor.g = ( TopCol.g * percent + BotCol.g * invPercent ) * invColorMultiAlpha;
	gradColor.b = ( TopCol.b * percent + BotCol.b * invPercent ) * invColorMultiAlpha;
	gradColor.a = ( TopCol.a * percent + BotCol.a * invPercent ) * invColorMultiAlpha;
	
	gl_FragColor.r = ( gradColor.r * gradColor.a ) + ( 1.0 - gradColor.a ) * gl_FragColor.r + (ColorAdd.r * ColorAdd.a);
	gl_FragColor.g = ( gradColor.g * gradColor.a ) + ( 1.0 - gradColor.a ) * gl_FragColor.g + (ColorAdd.g * ColorAdd.a);
	gl_FragColor.b = ( gradColor.b * gradColor.a ) + ( 1.0 - gradColor.a ) * gl_FragColor.b + (ColorAdd.b * ColorAdd.a);
}