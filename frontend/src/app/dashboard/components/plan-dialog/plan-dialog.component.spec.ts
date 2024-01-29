import { ComponentFixture, TestBed } from '@angular/core/testing';

import { PlanDialogComponent } from './plan-dialog.component';

describe('PlanDialogComponent', () => {
  let component: PlanDialogComponent;
  let fixture: ComponentFixture<PlanDialogComponent>;

  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [PlanDialogComponent]
    });
    fixture = TestBed.createComponent(PlanDialogComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
