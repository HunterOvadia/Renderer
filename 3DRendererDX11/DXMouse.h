#pragma once
#include <queue>
#include <optional>

class DXMouse
{
	friend class DXWindow;
public:
	class MouseEvent
	{
		public:
			enum class EType
			{
				LPress,
				LRelease,
				RPress,
				RRelease,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Exit,
			};

	public:
		MouseEvent(EType Type, const DXMouse& Parent) : Type(Type), 
			bIsLeftPressed(Parent.bIsLeftPressed), bIsRightPressed(Parent.bIsRightPressed),
			X(Parent.X), Y(Parent.Y) {}

		EType GetType() const { return Type; }
		std::pair<int, int> GetPos() const { return { X, Y }; }
		int GetPosX() const { return X; }
		int GetPosY() const { return Y; }
		bool IsLeftPressed() const { return bIsLeftPressed; }
		bool IsRightPressed() const { return bIsRightPressed; }

	private:
		EType Type;
		bool bIsLeftPressed;
		bool bIsRightPressed;
		int X;
		int Y;
	};

public:
	DXMouse() = default;
	DXMouse(const DXMouse&) = delete;
	DXMouse& operator=(const DXMouse&) = delete;
	std::pair<int, int> GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
	bool IsInWindow() const;
	bool IsLeftPressed() const;
	bool IsRightPressed() const;
	std::optional<MouseEvent> Read();
	void Flush();

	bool IsEmpty() const { return Buffer.empty(); }


private:
	void OnMouseMove(int X, int Y);
	void OnMouseEnter();
	void OnMouseExit();
	void OnLeftPressed(int X, int Y);
	void OnLeftReleased(int X, int Y);
	void OnRightPressed(int X, int Y);
	void OnRightReleased(int X, int Y);
	void OnWheelUp(int X, int Y);
	void OnWheelDown(int X, int Y);
	void TrimBuffer();
	void OnWheelDelta(int X, int Y, int Delta);

private:
	static constexpr unsigned int BufferSize = 16u;
	int X;
	int Y;
	bool bIsLeftPressed = false;
	bool bIsRightPressed = false;
	bool bIsInWindow = false;
	int WheelDeltaCarry = 0;
	std::queue<MouseEvent> Buffer;
};

