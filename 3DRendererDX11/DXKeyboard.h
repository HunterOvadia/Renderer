#pragma once
#include <queue>
#include <bitset>

class DXKeyboard
{
	friend class DXWindow;

public:
	class KeyboardEvent
	{
	public:
		enum class EType
		{
			Press,
			Release,
			Invalid
		};

	public:
		KeyboardEvent() : Type(EType::Invalid), Code(0u) {}
		KeyboardEvent(EType Type, unsigned char Code) : Type(Type), Code(Code) {}
		bool IsPressed() const { return Type == EType::Press; }
		bool IsReleased() const { return Type == EType::Release; }
		bool IsValid() const { return Type != EType::Invalid; }
		unsigned char GetCode() const { return Code; }
	private:
		EType Type;
		unsigned char Code;
	};

public:
	DXKeyboard() = default;
	DXKeyboard(const DXKeyboard&) = delete;
	DXKeyboard& operator=(const DXKeyboard&) = delete;

	bool IsKeyPressed(unsigned char KeyCode) const;
	KeyboardEvent ReadKey();
	bool IsKeyEmpty() const;
	void FlushKey();

	char ReadChar();
	bool IsCharEmpty() const;
	void FlushChar();
	void Flush();

	void EnableAutorepeat();
	void DisableAutorepeat();
	bool IsAutorepeatEnabled() const;

private:
	void OnKeyPressed(unsigned char KeyCode);
	void OnKeyReleased(unsigned char KeyCode);
	void OnChar(char Character);
	void ClearState();
	template<typename T>
	static void TrimBuffer(std::queue<T>& Buffer);

private:
	static constexpr unsigned int NumKeys = 256u;
	static constexpr unsigned int BufferSize = 16u;
	bool AutorepeatEnabled = false;
	std::bitset<NumKeys> KeyStates;
	std::queue<KeyboardEvent> KeyBuffer;
	std::queue<char> CharBuffer;
};