import { ComponentFixture, TestBed } from '@angular/core/testing';

import { GrundrissComponent } from './grundriss.component';

describe('GrundrissComponent', () => {
  let component: GrundrissComponent;
  let fixture: ComponentFixture<GrundrissComponent>;

  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [GrundrissComponent]
    });
    fixture = TestBed.createComponent(GrundrissComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
