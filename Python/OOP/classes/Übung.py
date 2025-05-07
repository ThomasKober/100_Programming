class Auto:

    def __init__(self, Pferdestärken, Farbe, Türen):
        self.Ps = Pferdestärken
        self.Fa = Farbe
        self.Tü = Türen

    @classmethod
    def auto_init(cls, Pferdestärken, Farbe, Türen):
        print(f'{cls} wurde erzeugt!')
        return  cls(Pferdestärken, Farbe, Türen)

    def auto_anzeigen(self):
        print(f' - PS: {self.Ps}\n - Farbe: {self.Fa}\n - Türen: {self.Tü}')

A1 = Auto.auto_init(110, "Grün", 2)

A1.auto_anzeigen()