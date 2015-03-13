using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace Aga.Controls.Tree.NodeControls
{
    public abstract class BaseTextControl : EditableControl
    {
        protected static Dictionary<Color, Brush> _brushes = new Dictionary<Color, Brush>();
        protected TextFormatFlags _baseFormatFlags;
        protected Pen _focusPen;
        protected StringFormat _format;
        protected TextFormatFlags _formatFlags;

        #region Properties

        protected bool _displayHiddenContentInToolTip = true;
        protected Font _font;

        protected HorizontalAlignment _textAlign = HorizontalAlignment.Left;

        protected StringTrimming _trimming = StringTrimming.None;
        protected bool _useCompatibleTextRendering;

        public Font Font
        {
            get
            {
                if (_font == null)
                    return Control.DefaultFont;
                else
                    return _font;
            }
            set
            {
                if (value == Control.DefaultFont)
                    _font = null;
                else
                    _font = value;
            }
        }

        [DefaultValue(HorizontalAlignment.Left)]
        public HorizontalAlignment TextAlign
        {
            get { return _textAlign; }
            set
            {
                _textAlign = value;
                SetFormatFlags();
            }
        }

        [DefaultValue(StringTrimming.None)]
        public StringTrimming Trimming
        {
            get { return _trimming; }
            set
            {
                _trimming = value;
                SetFormatFlags();
            }
        }

        [DefaultValue(true)]
        public bool DisplayHiddenContentInToolTip
        {
            get { return _displayHiddenContentInToolTip; }
            set { _displayHiddenContentInToolTip = value; }
        }

        [DefaultValue(false)]
        public bool UseCompatibleTextRendering
        {
            get { return _useCompatibleTextRendering; }
            set { _useCompatibleTextRendering = value; }
        }

        [DefaultValue(false)]
        public bool TrimMultiLine { get; set; }

        protected bool ShouldSerializeFont()
        {
            return (_font != null);
        }

        #endregion

        protected BaseTextControl()
        {
            IncrementalSearchEnabled = true;
            _focusPen = new Pen(Color.Black);
            _focusPen.DashStyle = DashStyle.Dot;

            _format =
                new StringFormat(StringFormatFlags.LineLimit | StringFormatFlags.NoClip | StringFormatFlags.FitBlackBox |
                                 StringFormatFlags.MeasureTrailingSpaces);
            _baseFormatFlags = TextFormatFlags.PreserveGraphicsClipping |
                               TextFormatFlags.PreserveGraphicsTranslateTransform;
            SetFormatFlags();
            LeftMargin = 3;
        }

        protected void SetFormatFlags()
        {
            _format.Alignment = TextHelper.TranslateAligment(TextAlign);
            _format.Trimming = Trimming;

            _formatFlags = _baseFormatFlags | TextHelper.TranslateAligmentToFlag(TextAlign)
                           | TextHelper.TranslateTrimmingToFlag(Trimming);
        }

        public override Size MeasureSize(TreeNodeAdv node, DrawContext context)
        {
            return GetLabelSize(node, context);
        }

        protected Size GetLabelSize(TreeNodeAdv node, DrawContext context)
        {
            return GetLabelSize(node, context, GetLabel(node));
        }

        protected Size GetLabelSize(TreeNodeAdv node, DrawContext context, string label)
        {
            CheckThread();
            Font font = GetDrawingFont(node, context, label);
            Size s = Size.Empty;
            if (UseCompatibleTextRendering)
                s = TextRenderer.MeasureText(label, font);
            else
            {
                SizeF sf = context.Graphics.MeasureString(label, font);
                s = new Size((int) Math.Ceiling(sf.Width), (int) Math.Ceiling(sf.Height));
            }

            if (!s.IsEmpty)
                return s;
            else
                return new Size(10, Font.Height);
        }

        protected Font GetDrawingFont(TreeNodeAdv node, DrawContext context, string label)
        {
            Font font = context.Font;
            if (DrawTextMustBeFired(node))
            {
                var args = new DrawEventArgs(node, this, context, label);
                args.Font = context.Font;
                OnDrawText(args);
                font = args.Font;
            }
            return font;
        }

        protected void SetEditControlProperties(Control control, TreeNodeAdv node)
        {
            string label = GetLabel(node);
            var context = new DrawContext();
            context.Font = control.Font;
            control.Font = GetDrawingFont(node, context, label);
        }

        public override void Draw(TreeNodeAdv node, DrawContext context)
        {
            if (context.CurrentEditorOwner == this && node == Parent.CurrentNode)
                return;

            string label = GetLabel(node);
            Rectangle bounds = GetBounds(node, context);
            var focusRect = new Rectangle(bounds.X, context.Bounds.Y,
                                          bounds.Width, context.Bounds.Height);

            Brush backgroundBrush;
            Color textColor;
            Font font;
            CreateBrushes(node, context, label, out backgroundBrush, out textColor, out font, ref label);

            if (backgroundBrush != null)
                context.Graphics.FillRectangle(backgroundBrush, focusRect);
            if (context.DrawFocus)
            {
                focusRect.Width--;
                focusRect.Height--;
                if (!IsCategoryNode(node))
                {
                    if (context.DrawSelection == DrawSelectionMode.None)
                        _focusPen.Color = SystemColors.ControlText;
                    else
                        _focusPen.Color = SystemColors.InactiveCaption;
                    context.Graphics.DrawRectangle(_focusPen, focusRect);
                }
            }

            if (UseCompatibleTextRendering)
                TextRenderer.DrawText(context.Graphics, label, font, bounds, textColor, _formatFlags);
            else
                context.Graphics.DrawString(label, font, GetBrush(textColor), bounds, _format);
        }

        private bool IsCategoryNode(TreeNodeAdv node)
        {
            return node.Tag is Node && ((Node) node.Tag).IsCategoryNode;
        }

        protected static Brush GetBrush(Color color)
        {
            Brush br;
            if (_brushes.ContainsKey(color))
                br = _brushes[color];
            else
            {
                br = new SolidBrush(color);
                _brushes.Add(color, br);
            }
            return br;
        }

        protected virtual void CreateBrushes(TreeNodeAdv node, DrawContext context, string text,
                                             out Brush backgroundBrush, out Color textColor, out Font font,
                                             ref string label)
        {
            textColor = SystemColors.ControlText;
            backgroundBrush = null;
            font = context.Font;

            var n = (Node) node.Tag;
            if (n.ForeColorSet)
                textColor = n.ForeColor;
            if (n.BoldSet && n.Bold)
                font = new Font(font, FontStyle.Bold);


            switch (context.DrawSelection)
            {
                case DrawSelectionMode.Inactive:
                case DrawSelectionMode.Active:
                    if (!n.IsCategoryNode)
                    {
                        textColor = SystemColors.HighlightText;
                        backgroundBrush = SystemBrushes.Highlight;
                    }
                    break;
                case DrawSelectionMode.FullRowSelect:
                    if (!n.IsCategoryNode) textColor = SystemColors.HighlightText;
                    break;
                default:
                    if (n.Brush != null)
                        backgroundBrush = n.Brush;
                    break;
            }
            if (!context.Enabled)
                textColor = SystemColors.GrayText;

            if (DrawTextMustBeFired(node))
            {
                var args = new DrawEventArgs(node, this, context, text);
                args.Text = label;
                args.TextColor = textColor;
                args.BackgroundBrush = backgroundBrush;
                args.Font = font;

                OnDrawText(args);

                textColor = args.TextColor;
                backgroundBrush = args.BackgroundBrush;
                font = args.Font;
                label = args.Text;
            }

            //font = new Font(context.Font.Name, context.Font.Size + 5.0F, context.Font.Style, context.Font.Unit);
            //context.Font = font;
        }

        public virtual string GetLabel(TreeNodeAdv node)
        {
            if (node != null && node.Tag != null)
            {
                object obj = GetValue(node);
                if (obj != null)
                    return FormatLabel(obj);
            }
            return string.Empty;
        }

        protected virtual string FormatLabel(object obj)
        {
            string res = obj.ToString();
            if (TrimMultiLine && res != null)
            {
                string[] parts = res.Split('\n');
                if (parts.Length > 1)
                    return parts[0] + "...";
            }
            return res;
        }

        public void SetLabel(TreeNodeAdv node, string value)
        {
            SetValue(node, value);
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
            if (disposing)
            {
                _focusPen.Dispose();
                _format.Dispose();
            }
        }

        /// <summary>
        /// Fires when control is going to draw a text. Can be used to change text or back color
        /// </summary>
        public event EventHandler<DrawEventArgs> DrawText;

        protected virtual void OnDrawText(DrawEventArgs args)
        {
            TreeViewAdv tree = args.Node.Tree;
            if (tree != null)
                tree.FireDrawControl(args);
            if (DrawText != null)
                DrawText(this, args);
        }

        protected virtual bool DrawTextMustBeFired(TreeNodeAdv node)
        {
            return DrawText != null || (node.Tree != null && node.Tree.DrawControlMustBeFired());
        }
    }
}