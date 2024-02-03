import { ComponentFixture, TestBed } from '@angular/core/testing';

import { PlansComponent } from './plans.component';

describe('GrundrissComponent', () => {
  let component: PlansComponent;
  let fixture: ComponentFixture<PlansComponent>;

  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [PlansComponent]
    });
    fixture = TestBed.createComponent(PlansComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
