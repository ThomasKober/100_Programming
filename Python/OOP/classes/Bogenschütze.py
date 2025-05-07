class Archer:

    def __init__(self,na, ar, st, he):
        self.name = na
        self.arrows = ar
        self.steps = st
        self.health = he

    def shoot(self):
        if self.arrows > 0:
            print(f'{self.name} shoots')
            self.arrows -= 1
        else:
            print(f'{self.name} is out of arrows!')


    def walk(self):
        if self.steps > 0:
            print(f'{self.name} walks')
            self.steps -= 1
        else:
            print(f'{self.name} ca not walk anymore')


    def damage(self):
        if self.health:
            print(f'{self.name} got hit')
            self.health -= 1
        else:
            print(f'{self.name} is dead!')

    @classmethod
    def cls_methode(cls):
        print(f'I am a Class Methode {cls}')

    @staticmethod
    def stat_methode():
        print('I am a static Methode')

    @classmethod
    def player_init(cls, na, ar, st, he):
        print(f'{cls} was created')
        return cls(na, ar, st, he)


A1 = Archer("Tom",5, 10, 5)
A2 = Archer("Jerry",5, 10, 5)
A4 = Archer.player_init("Pluto", 5, 5, 5)

print("Archer1:")
print(f" - Arrows = {A1.arrows}\n - Steps = {A1.steps}\n - Health = {A1.health}")

print("Archer2")
print(f" - Pfeile = {A2.arrows}\n - Steps = {A2.steps}\n - Health = {A2.health} ")

A1.shoot()
A1.shoot()
A1.shoot()
A1.shoot()
A1.shoot()
A1.shoot()

A1.cls_methode()

A1.stat_methode()

A3 = Archer
A3.stat_methode()

A4.shoot()



